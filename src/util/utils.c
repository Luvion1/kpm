#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h> // Fix for WIFEXITED

char* read_file(const char* filepath) {
    FILE* file = fopen(filepath, "rb");
    if (file == NULL) return NULL;

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Validasi ukuran file untuk mencegah alokasi memori besar
    if (fileSize < 0) {
        fclose(file);
        return NULL;
    }

    // Batasi ukuran file maksimum (misalnya 100MB)
    if (fileSize > 100 * 1024 * 1024) {
        fclose(file);
        return NULL;
    }

    char* buffer = malloc(fileSize + 1);
    if (buffer == NULL) {
        fclose(file);
        return NULL;
    }

    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    buffer[bytesRead] = '\0';
    fclose(file);
    return buffer;
}

bool fs_mkdir_p(const char* path) {
    if (!path) return false;

    size_t len = strlen(path);
    if (len == 0) return true;  // path kosong tidak perlu dibuat

    char* temp_path = malloc(len + 1);  // Alokasi dinamis berdasarkan panjang path
    if (!temp_path) return false;

    strncpy(temp_path, path, len + 1);

    if (temp_path[len - 1] == '/') temp_path[len - 1] = 0;

    for (char* p = temp_path + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            if (mkdir(temp_path, 0755) != 0 && errno != EEXIST) {
                free(temp_path);
                return false;
            }
            *p = '/';
        }
    }
    if (mkdir(temp_path, 0755) != 0 && errno != EEXIST) {
        free(temp_path);
        return false;
    }

    free(temp_path);
    return true;
}

bool run_shell_cmd(const char* cmd) {
    int status = system(cmd);
    if (status == -1) return false;
    return (WIFEXITED(status) && WEXITSTATUS(status) == 0);
}

bool is_path_safe(const char* path) {
    if (!path) return false;

    // Cek untuk directory traversal (..)
    if (strstr(path, "..") != NULL) return false;

    // Cek untuk karakter berbahaya
    const char* dangerous_chars = "<>\"|?*";
    for (size_t i = 0; i < strlen(dangerous_chars); i++) {
        if (strchr(path, dangerous_chars[i]) != NULL) return false;
    }

    return true;
}