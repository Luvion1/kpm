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

    if (!fs_mkdir_p(destination_dir)) return false;
    rmdir(destination_dir); 

    char cmd[1024];
    if (version && strcmp(version, "HEAD") != 0) {
        snprintf(cmd, sizeof(cmd), "git clone --depth 1 --branch %s https://%s %s > /dev/null 2>&1", 
                 version, package_url, destination_dir);
    } else {
        snprintf(cmd, sizeof(cmd), "git clone --depth 1 https://%s %s > /dev/null 2>&1", 
                 package_url, destination_dir);
    }

    if (!run_shell_cmd(cmd)) return false;

    char git_folder[1024];
    snprintf(git_folder, sizeof(git_folder), "rm -rf %s/.git", destination_dir);
    run_shell_cmd(git_folder);

    return true;
}

