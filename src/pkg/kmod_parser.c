#include "kmod_parser.h"
#include "../util/utils.h"
#include <ctype.h>
#include <string.h>
#include <stddef.h>

// Fungsi untuk melewati whitespace
static const char* skip_whitespace(const char* str) {
    while (*str && isspace(*str)) {
        str++;
    }
    return str;
}

// Fungsi untuk memeriksa apakah string dimulai dengan prefix
static bool starts_with(const char* str, const char* prefix) {
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

// Fungsi untuk mengambil string hingga karakter tertentu
static char* extract_until(const char* start, const char* end) {
    if (!start || !end || start >= end) return NULL;

    ptrdiff_t len = end - start;  // Gunakan ptrdiff_t untuk mencegah overflow
    if (len <= 0) return NULL;

    // Batasi panjang maksimum untuk mencegah alokasi memori besar
    if (len > 1024) return NULL;

    char* result = malloc(len + 1);
    if (!result) return NULL;

    strncpy(result, start, len);
    result[len] = '\0';
    return result;
}

// Parsing file kmod
KModInfo* parse_kmod_file(const char* filepath) {
    char* content = read_file(filepath);
    if (!content) {
        return NULL;
    }

    KModInfo* kmod = malloc(sizeof(KModInfo));
    if (!kmod) {
        free(content);
        return NULL;
    }

    kmod->module_name = NULL;
    kmod->ksharp_version = NULL;
    kmod->requirements = NULL;
    kmod->req_count = 0;
    kmod->req_capacity = 0;

    const char* ptr = content;
    const char* line_start = ptr;

    while (*ptr) {
        // Temukan akhir baris
        const char* line_end = ptr;
        while (*line_end && *line_end != '\n' && *line_end != '\r') {
            line_end++;
        }

        // Lewati whitespace di awal baris
        const char* line = skip_whitespace(line_start);

        // Cek apakah baris kosong atau komentar
        if (*line == '\n' || *line == '\r' || *line == '/' || *line == '\0') {
            ptr = line_end;
            if (*ptr) ptr++;
            line_start = ptr;
            continue;
        }

        // Cek jenis perintah
        if (starts_with(line, "module ")) {
            const char* module_start = line + 7; // "module " length
            module_start = skip_whitespace(module_start);
            const char* module_end = module_start;
            while (*module_end && !isspace(*module_end) && *module_end != '\n' && *module_end != '\r') {
                module_end++;
            }
            
            kmod->module_name = extract_until(module_start, module_end);
        }
        else if (starts_with(line, "ksharp ")) {
            const char* version_start = line + 7; // "ksharp " length
            version_start = skip_whitespace(version_start);
            const char* version_end = version_start;
            while (*version_end && !isspace(*version_end) && *version_end != '\n' && *version_end != '\r') {
                version_end++;
            }
            
            kmod->ksharp_version = extract_until(version_start, version_end);
        }
        else if (starts_with(line, "require (")) {
            // Pindai bagian require - skip the opening line "require ("
            // Advance past the current line first
            ptr = line_end;
            if (*ptr) ptr++;
            line_start = ptr;

            // Cari sampai tutup kurung
            while (*ptr) {
                const char* req_line_start = ptr;
                const char* req_line_end = ptr;

                while (*req_line_end && *req_line_end != '\n' && *req_line_end != '\r') {
                    req_line_end++;
                }

                const char* req_line = skip_whitespace(req_line_start);

                // Check if this is the closing parenthesis line (contains only ')')
                // First check if first char is ')', then make sure it's the only non-whitespace char
                if (req_line && *req_line == ')') {
                    // Check if there are only whitespace characters after the ')'
                    const char* after_paren = req_line + 1;
                    bool only_whitespace_after = true;
                    while (*after_paren) {
                        if (!isspace(*after_paren)) {
                            only_whitespace_after = false;
                            break;
                        }
                        after_paren++;
                    }

                    if (only_whitespace_after) {
                        // Akhir dari blok require
                        ptr = req_line_end;
                        if (*ptr) ptr++;
                        line_start = ptr;
                        break;
                    }
                }

                // Parse require entry - skip if it's just whitespace, comment, or the opening line "require ("
                if (req_line && *req_line != '\0' && *req_line != '\n' && *req_line != '\r') {
                    // Skip comment lines that start with //
                    if (starts_with(req_line, "//")) {
                        ptr = req_line_end;
                        if (*ptr) ptr++;
                        line_start = ptr;
                        continue;
                    }

                    // Skip the opening line "require (" itself
                    const char* trimmed_line = skip_whitespace(req_line);
                    if (starts_with(trimmed_line, "require (")) {
                        ptr = req_line_end;
                        if (*ptr) ptr++;
                        line_start = ptr;
                        continue;
                    }

                    // Cari nama modul dan versi
                    const char* name_start = skip_whitespace(req_line);
                    if (!name_start || *name_start == '\0') {
                        ptr = req_line_end;
                        if (*ptr) ptr++;
                        line_start = ptr;
                        continue;
                    }

                    const char* name_end = name_start;
                    while (*name_end && !isspace(*name_end) && *name_end != '\n' && *name_end != '\r') {
                        name_end++;
                    }

                    const char* version_start = skip_whitespace(name_end);
                    if (!version_start || version_start >= req_line_end || *version_start == '\0') {
                        ptr = req_line_end;
                        if (*ptr) ptr++;
                        line_start = ptr;
                        continue; // Skip if no version is found
                    }

                    const char* version_end = version_start;
                    while (*version_end && !isspace(*version_end) && *version_end != '\n' && *version_end != '\r') {
                        version_end++;
                    }

                    if (name_end > name_start && version_end > version_start) {
                        // Buat string "module version"
                        int name_len = name_end - name_start;
                        int version_len = version_end - version_start;
                        char* req_entry = malloc(name_len + version_len + 2); // +1 for space, +1 for null
                        if (req_entry) {
                            strncpy(req_entry, name_start, name_len);
                            req_entry[name_len] = ' ';
                            strncpy(req_entry + name_len + 1, version_start, version_len);
                            req_entry[name_len + 1 + version_len] = '\0';

                            // Tambahkan ke array requirements
                            if (kmod->req_count >= kmod->req_capacity) {
                                int new_capacity = kmod->req_capacity ? kmod->req_capacity * 2 : 4;
                                char** new_reqs = realloc(kmod->requirements, new_capacity * sizeof(char*));
                                if (new_reqs) {
                                    kmod->requirements = new_reqs;
                                    kmod->req_capacity = new_capacity;
                                } else {
                                    free(req_entry);
                                    break;
                                }
                            }

                            if (kmod->req_capacity > 0) {
                                kmod->requirements[kmod->req_count] = req_entry;
                                kmod->req_count++;
                            } else {
                                free(req_entry);
                            }
                        }
                    }
                }

                ptr = req_line_end;
                if (*ptr) ptr++;
                line_start = ptr;
            }
            continue; // Lanjut ke iterasi berikutnya karena kita sudah maju pointer
        }

        ptr = line_end;
        if (*ptr) ptr++;
        line_start = ptr;
    }

    free(content);
    return kmod;
}

// Dealokasi kmod info
void free_kmod_info(KModInfo* kmod_info) {
    if (!kmod_info) return;

    free(kmod_info->module_name);
    free(kmod_info->ksharp_version);

    if (kmod_info->requirements) {
        for (int i = 0; i < kmod_info->req_count; i++) {
            free(kmod_info->requirements[i]);
        }
        free(kmod_info->requirements);
    }

    free(kmod_info);
}

