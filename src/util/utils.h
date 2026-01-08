#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

// Membaca konten file ke string
char* read_file(const char* filepath);

// Membuat direktori secara rekursif (mkdir -p)
bool fs_mkdir_p(const char* path);

// Mengeksekusi perintah shell. Return true jika exit code 0.
bool run_shell_cmd(const char* cmd);

// Validasi path untuk mencegah directory traversal (..)
bool is_path_safe(const char* path);

#endif
