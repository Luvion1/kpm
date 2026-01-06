#include "stateless_registry.h"
#include "../util/utils.h"
#include "../pkg/visual_utils.h"
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>

// === CONFIG ===
RegistryConfig* create_registry_config(const char* registry_url, const char* storage_path) {
    RegistryConfig* config = malloc(sizeof(RegistryConfig));
    if (!config) return NULL;
    config->registry_url = strdup(registry_url ? registry_url : "direct");
    config->storage_path = strdup(storage_path ? storage_path : "/tmp/ksharp_cache");
    config->max_package_size = 100;
    config->enable_caching = true;
    config->cache_ttl = 3600;
    config->allowed_hosts = NULL;
    return config;
}

void free_registry_config(RegistryConfig* config) {
    if (!config) return;
    free(config->registry_url); free(config->storage_path); free(config->allowed_hosts);
    free(config);
}

// === PACKAGE STRUCT ===

void free_registry_package(RegistryPackage* pkg) {
    if (!pkg) return;
    free(pkg->name); free(pkg->version); free(pkg->description);
    free(pkg->author); free(pkg->license); free(pkg->source_url);
    free(pkg->checksum); free(pkg->created_at);
    if (pkg->dependencies) {
        for (int i = 0; i < pkg->dep_count; i++) free(pkg->dependencies[i]);
        free(pkg->dependencies);
    }
    free(pkg);
}

// === REGISTRY LIFECYCLE ===
StatelessRegistry* init_stateless_registry(const char* registry_url, const char* storage_path) {
    StatelessRegistry* registry = malloc(sizeof(StatelessRegistry));
    if (!registry) return NULL;
    registry->config = create_registry_config(registry_url, storage_path);
    if (!registry->config) { free(registry); return NULL; }
    
    fs_mkdir_p(registry->config->storage_path);
    
    registry->packages = NULL;
    registry->package_count = 0;
    return registry;
}

void free_stateless_registry(StatelessRegistry* registry) {
    if (!registry) return;
    RegistryPackage* current = registry->packages;
    while (current) {
        RegistryPackage* next = current->next;
        free_registry_package(current);
        current = next;
    }
    free_registry_config(registry->config);
    free(registry);
}

// === REAL IMPLEMENTATION ===

bool registry_download_package(const StatelessRegistry* registry, const char* package_url, const char* version, const char* destination_dir) {
    if (!registry || !package_url || !destination_dir) return false;

    // Validasi input untuk mencegah command injection
    if (strchr(package_url, '\'') || strchr(package_url, '"') || strchr(package_url, ';') ||
        strchr(package_url, '&') || strchr(package_url, '|') || strchr(package_url, '$') ||
        strchr(package_url, '`') || strchr(package_url, '(') || strchr(package_url, ')')) {
        show_status_message("Invalid characters in package URL", "error");
        return false;
    }

    if (version && (strchr(version, '\'') || strchr(version, '"') || strchr(version, ';') ||
                    strchr(version, '&') || strchr(version, '|') || strchr(version, '$') ||
                    strchr(version, '`') || strchr(version, '(') || strchr(version, ')'))) {
        show_status_message("Invalid characters in version", "error");
        return false;
    }

    // Validasi destination_dir juga
    if (strchr(destination_dir, '\'') || strchr(destination_dir, '"') || strchr(destination_dir, ';') ||
        strchr(destination_dir, '&') || strchr(destination_dir, '|') || strchr(destination_dir, '$') ||
        strchr(destination_dir, '`')) {
        show_status_message("Invalid characters in destination directory", "error");
        return false;
    }

    // Buat direktori tujuan
    if (!fs_mkdir_p(destination_dir)) return false;

    // Deteksi dan tangani protokol secara cerdas
    const char* effective_url = package_url;
    char cmd[2048];  // Meningkatkan ukuran buffer untuk keamanan

    // Cek apakah URL sudah memiliki protokol (http://, https://, git@, dll.)
    if (strncmp(package_url, "http://", 7) == 0 ||
        strncmp(package_url, "https://", 8) == 0 ||
        strncmp(package_url, "git@", 4) == 0 ||
        strncmp(package_url, "ssh://", 6) == 0) {
        // Jika sudah memiliki protokol, gunakan URL asli
        if (version && strcmp(version, "HEAD") != 0) {
            // Gunakan perintah shell yang lebih aman dengan escaping
            int written = snprintf(cmd, sizeof(cmd), "git clone --depth 1 --branch '%s' '%s' '%s' > /dev/null 2>&1",
                     version, effective_url, destination_dir);
            if (written < 0 || (size_t)written >= sizeof(cmd)) {
                show_status_message("Command too long", "error");
                return false;
            }
        } else {
            int written = snprintf(cmd, sizeof(cmd), "git clone --depth 1 '%s' '%s' > /dev/null 2>&1",
                     effective_url, destination_dir);
            if (written < 0 || (size_t)written >= sizeof(cmd)) {
                show_status_message("Command too long", "error");
                return false;
            }
        }
    } else {
        // Jika tidak memiliki protokol, tambahkan https:// secara default
        if (version && strcmp(version, "HEAD") != 0) {
            int written = snprintf(cmd, sizeof(cmd), "git clone --depth 1 --branch '%s' 'https://%s' '%s' > /dev/null 2>&1",
                     version, effective_url, destination_dir);
            if (written < 0 || (size_t)written >= sizeof(cmd)) {
                show_status_message("Command too long", "error");
                return false;
            }
        } else {
            int written = snprintf(cmd, sizeof(cmd), "git clone --depth 1 'https://%s' '%s' > /dev/null 2>&1",
                     effective_url, destination_dir);
            if (written < 0 || (size_t)written >= sizeof(cmd)) {
                show_status_message("Command too long", "error");
                return false;
            }
        }
    }

    if (!run_shell_cmd(cmd)) return false;

    char git_folder[1024];
    int written = snprintf(git_folder, sizeof(git_folder), "rm -rf '%s/.git'", destination_dir);
    if (written < 0 || (size_t)written >= sizeof(git_folder)) {
        show_status_message("Command too long", "error");
        return false;
    }
    run_shell_cmd(git_folder);

    return true;
}

