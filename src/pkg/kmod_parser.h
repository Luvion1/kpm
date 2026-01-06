#ifndef KMOD_PARSER_H
#define KMOD_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct KModInfo {
    char* module_name;
    char* ksharp_version;
    char** requirements;
    int req_count;
    int req_capacity;
} KModInfo;

KModInfo* parse_kmod_file(const char* filepath);
void free_kmod_info(KModInfo* kmod_info);


#endif
