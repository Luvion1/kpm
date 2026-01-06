#ifndef STATELESS_REGISTRY_H
#define STATELESS_REGISTRY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct RegistryPackage {
    char* name;
    char* version;
    char* description;
    char* author;
    char* license;
    char* source_url;
    char* checksum;
    long size;
    char* created_at;
    char** dependencies;
    int dep_count;
    struct RegistryPackage* next;
} RegistryPackage;

typedef struct RegistryConfig {
    char* registry_url;
    char* storage_path;
    int max_package_size;
    bool enable_caching;
    int cache_ttl;
    char* allowed_hosts;
} RegistryConfig;

typedef struct StatelessRegistry {
    RegistryConfig* config;
    RegistryPackage* packages;
    int package_count;
} StatelessRegistry;

StatelessRegistry* init_stateless_registry(const char* registry_url, const char* storage_path);
void free_stateless_registry(StatelessRegistry* registry);
RegistryPackage* create_registry_package(const char* name, const char* version);
void free_registry_package(RegistryPackage* pkg);

bool registry_download_package(const StatelessRegistry* registry, const char* package_name, const char* version, const char* destination);

#endif