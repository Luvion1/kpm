#ifndef UNIFIED_PACKAGE_MANAGER_H
#define UNIFIED_PACKAGE_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include "visual_utils.h"
#include "stateless_registry.h"
#include "kmod_parser.h"

// Struktur UnifiedPackage menggabungkan informasi dari berbagai sumber
typedef struct UnifiedPackage {
    char* name;
    char* version;
    char* description;
    char* author;
    char* license;
    char* source_url;
    char* checksum;
    long size;
    char* created_at;
    char* install_path;
    char** dependencies;
    int dep_count;
    struct UnifiedPackage* next;
} UnifiedPackage;

// Struktur UnifiedPackageManager menggabungkan semua komponen
typedef struct UnifiedPackageManager {
    char* root_dir;             // Direktori root project
    char* cache_dir;            // Direktori cache global
    char* registry_url;         // URL registry
    UnifiedPackage* packages;   // Daftar package terinstal
    int package_count;
    StatelessRegistry* registry; // Koneksi ke registry
    // Konfigurasi tambahan
    int max_package_size;       // Ukuran maksimum package (MB)
    bool enable_caching;        // Apakah caching diaktifkan
    int cache_ttl;             // Cache TTL dalam detik
} UnifiedPackageManager;

// Fungsi-fungsi utama untuk Unified Package Manager
UnifiedPackageManager* init_unified_package_manager(const char* root_dir, const char* registry_url);
void free_unified_package_manager(UnifiedPackageManager* upm);

// Fungsi-fungsi untuk manajemen package
UnifiedPackage* create_unified_package(const char* name, const char* version);
void free_unified_package(UnifiedPackage* pkg);
bool install_package(UnifiedPackageManager* upm, const char* package_name, const char* version);
bool uninstall_package(UnifiedPackageManager* upm, const char* package_name);
bool list_packages(UnifiedPackageManager* upm);

// Fungsi-fungsi untuk manajemen proyek
bool create_project(const char* project_name);
bool tidy_dependencies(UnifiedPackageManager* upm);

// Fungsi-fungsi untuk manajemen metadata header-only
typedef struct HeaderMeta {
    char* package_name;
    char* version;
    char* description;
    char* author;
    char* license;
    char* dependencies;
    char* keywords;
    char* homepage;
    char* repository;
    char* bugs;
    char* contributors;
    char* categories;
    char* tags;
} HeaderMeta;

HeaderMeta* create_header_meta();
void free_header_meta(HeaderMeta* meta);
bool parse_header_meta_from_file(const char* filepath, HeaderMeta* meta);
bool save_header_meta_to_file(const char* filepath, const HeaderMeta* meta);

// Fungsi-fungsi utilitas
char* read_file_safe(const char* filepath);
bool write_file_safe(const char* filepath, const char* content);
bool directory_exists(const char* path);
bool create_directory_recursive(const char* path);

#endif
