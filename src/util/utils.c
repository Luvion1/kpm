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
    char* buffer = malloc(fileSize + 1);
    if (buffer == NULL) { fclose(file); return NULL; }
    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    buffer[bytesRead] = '\0';
    fclose(file);
    return buffer;
}

bool fs_mkdir_p(const char* path) {
    char temp_path[512];
    char* p = NULL;
    snprintf(temp_path, sizeof(temp_path), "%s", path);
    size_t len = strlen(temp_path);
    if (temp_path[len - 1] == '/') temp_path[len - 1] = 0;
    
    for (p = temp_path + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            if (mkdir(temp_path, 0755) != 0 && errno != EEXIST) return false;
            *p = '/';
        }
    }
    if (mkdir(temp_path, 0755) != 0 && errno != EEXIST) return false;
    return true;
}

bool run_shell_cmd(const char* cmd) {
    int status = system(cmd);
    if (status == -1) return false;
    return (WIFEXITED(status) && WEXITSTATUS(status) == 0);
}