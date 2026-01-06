#include "unified_package_manager.h"
#include "visual_utils.h"
#include "kmod_parser.h"
#include <errno.h>
#include <ctype.h>

// Fungsi-fungsi utilitas
char* read_file_safe(const char* filepath) {
    if (!filepath) return NULL;

    FILE* fp = fopen(filepath, "r");
    if (!fp) return NULL;

    fseek(fp, 0, SEEK_END);
    long length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Validasi ukuran file untuk mencegah alokasi memori besar
    if (length < 0) {
        fclose(fp);
        return NULL;
    }

    // Batasi ukuran file maksimum (misalnya 100MB)
    if (length > 100 * 1024 * 1024) {
        fclose(fp);
        return NULL;
    }

    char* content = malloc(length + 1);
    if (!content) {
        fclose(fp);
        return NULL;
    }

    size_t read_size = fread(content, 1, length, fp);
    if (read_size != (size_t)length) {
        free(content);
        fclose(fp);
        return NULL;
    }

    content[length] = '\0';
    fclose(fp);
    return content;
}

// bool write_file_safe(const char* filepath, const char* content) {
//     if (!filepath || !content) return false;
//
//     FILE* fp = fopen(filepath, "w");
//     if (!fp) return false;
//
//     size_t len = strlen(content);
//     size_t written = fwrite(content, 1, len, fp);
//     fclose(fp);
//
//     return written == len;
// }

bool directory_exists(const char* path) {
    if (!path) return false;
    
    struct stat st;
    return (stat(path, &st) == 0 && S_ISDIR(st.st_mode));
}

bool create_directory_recursive(const char* path) {
    if (!path) return false;

    char tmp[4096];  // Meningkatkan ukuran buffer untuk mencegah overflow
    size_t len = strlen(path);

    // Cek apakah path terlalu panjang
    if (len >= sizeof(tmp)) {
        return false;
    }

    strcpy(tmp, path);

    if (tmp[len - 1] == '/') tmp[len - 1] = 0;
    for (char* p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            if (mkdir(tmp, S_IRWXU) != 0 && errno != EEXIST) {
                return false;
            }
            *p = '/';
        }
    }
    if (mkdir(tmp, S_IRWXU) != 0 && errno != EEXIST) {
        return false;
    }
    return true;
}

// Fungsi-fungsi untuk UnifiedPackage
UnifiedPackage* create_unified_package(const char* name, const char* version) {
    UnifiedPackage* pkg = malloc(sizeof(UnifiedPackage));
    if (!pkg) return NULL;
    
    pkg->name = name ? strdup(name) : NULL;
    pkg->version = version ? strdup(version) : NULL;
    pkg->description = NULL;
    pkg->author = NULL;
    pkg->license = NULL;
    pkg->source_url = NULL;
    pkg->checksum = NULL;
    pkg->size = 0;
    pkg->created_at = NULL;
    pkg->install_path = NULL;
    pkg->dependencies = NULL;
    pkg->dep_count = 0;
    pkg->next = NULL;
    
    return pkg;
}

void free_unified_package(UnifiedPackage* pkg) {
    if (!pkg) return;
    
    free(pkg->name);
    free(pkg->version);
    free(pkg->description);
    free(pkg->author);
    free(pkg->license);
    free(pkg->source_url);
    free(pkg->checksum);
    free(pkg->created_at);
    free(pkg->install_path);
    
    if (pkg->dependencies) {
        for (int i = 0; i < pkg->dep_count; i++) {
            free(pkg->dependencies[i]);
        }
        free(pkg->dependencies);
    }
    
    free(pkg);
}

// Fungsi-fungsi untuk UnifiedPackageManager
UnifiedPackageManager* init_unified_package_manager(const char* root_dir, const char* registry_url) {
    UnifiedPackageManager* upm = malloc(sizeof(UnifiedPackageManager));
    if (!upm) return NULL;
    
    upm->root_dir = root_dir ? strdup(root_dir) : NULL;
    upm->cache_dir = strdup("/tmp/ksharp_cache"); // Default cache dir
    upm->registry_url = registry_url ? strdup(registry_url) : strdup("direct");
    upm->packages = NULL;
    upm->package_count = 0;
    upm->max_package_size = 100; // Default 100MB
    upm->enable_caching = true;
    upm->cache_ttl = 3600; // Default 1 hour
    upm->registry = init_stateless_registry(upm->registry_url, upm->cache_dir);
    
    // Pastikan direktori cache ada
    if (!directory_exists(upm->cache_dir)) {
        if (!create_directory_recursive(upm->cache_dir)) {
            free_unified_package_manager(upm);
            return NULL;
        }
    }
    
    return upm;
}

void free_unified_package_manager(UnifiedPackageManager* upm) {
    if (!upm) return;
    
    // Bebaskan semua package
    UnifiedPackage* current = upm->packages;
    while (current) {
        UnifiedPackage* next = current->next;
        free_unified_package(current);
        current = next;
    }
    
    if (upm->registry) free_stateless_registry(upm->registry);
    free(upm->root_dir);
    free(upm->cache_dir);
    free(upm->registry_url);
    free(upm);
}

// Fungsi-fungsi manajemen package
bool install_package(UnifiedPackageManager* upm, const char* package_name, const char* version) {
    if (!upm || !package_name) return false;

    // Validasi panjang nama package untuk mencegah buffer overflow
    size_t package_name_len = strlen(package_name);
    if (package_name_len == 0) {
        show_status_message("Package name cannot be empty", "error");
        return false;
    }

    // Maksimum panjang nama package agar path tetap dalam batas aman
    if (package_name_len > 500) {
        show_status_message("Package name too long (max 500 characters)", "error");
        return false;
    }

    // Cek apakah package sudah terinstal
    UnifiedPackage* current = upm->packages;
    while (current) {
        if (strcmp(current->name, package_name) == 0) {
            char msg[256];
            // Gunakan snprintf dengan batas aman untuk mencegah buffer overflow
            int written = snprintf(msg, sizeof(msg), "Package %s is already installed", package_name);
            if (written < 0 || (size_t)written >= sizeof(msg)) {
                // Jika pesan terpotong, gunakan pesan generik
                show_status_message("Package is already installed", "warning");
            } else {
                show_status_message(msg, "warning");
            }
            return true; // Sudah terinstal, tidak error
        }
        current = current->next;
    }

    // Tampilkan animasi instalasi
    show_installing_animation(package_name);

    // Download secara nyata
    char install_path[1024];
    // Validasi bahwa kombinasi cache_dir dan package_name tidak melebihi batas aman
    size_t cache_dir_len = strlen(upm->cache_dir);
    if (cache_dir_len + package_name_len + 1 >= sizeof(install_path)) {  // +1 untuk '/'
        show_status_message("Cache directory path too long", "error");
        return false;
    }

    snprintf(install_path, sizeof(install_path), "%s/%s", upm->cache_dir, package_name);

    if (!registry_download_package(upm->registry, package_name, version, install_path)) {
        show_status_message("Download failed", "error");
        return false;
    }

    // Buat package baru
    UnifiedPackage* new_pkg = create_unified_package(package_name, version ? version : "latest");
    if (!new_pkg) {
        show_status_message("Failed to create package metadata", "error");
        return false;
    }
    new_pkg->install_path = strdup(install_path);

    // Tambahkan ke daftar
    new_pkg->next = upm->packages;
    upm->packages = new_pkg;
    upm->package_count++;

    printf(COL_BRIGHT_GREEN "\n  ✅ Package installed successfully!\n" COL_RESET);
    return true;
}

bool uninstall_package(UnifiedPackageManager* upm, const char* package_name) {
    if (!upm || !package_name) return false;

    // Validasi panjang nama package untuk mencegah buffer overflow
    size_t package_name_len = strlen(package_name);
    if (package_name_len == 0) {
        show_status_message("Package name cannot be empty", "error");
        return false;
    }

    // Maksimum panjang nama package agar pesan tetap dalam batas aman
    if (package_name_len > 200) {
        show_status_message("Package name too long (max 200 characters)", "error");
        return false;
    }

    UnifiedPackage* current = upm->packages;
    UnifiedPackage* prev = NULL;

    // Tampilkan animasi loading untuk proses uninstall
    char msg[256];
    // Gunakan snprintf dengan batas aman untuk mencegah buffer overflow
    int written = snprintf(msg, sizeof(msg), COL_BRIGHT_RED "Uninstalling package: %s" COL_RESET, package_name);
    if (written < 0 || (size_t)written >= sizeof(msg)) {
        // Jika pesan terpotong, gunakan pesan generik
        show_loading_animation(COL_BRIGHT_RED "Uninstalling package" COL_RESET, 2);
    } else {
        show_loading_animation(msg, 2);
    }

    while (current) {
        if (strcmp(current->name, package_name) == 0) {
            if (prev) {
                prev->next = current->next;
            } else {
                upm->packages = current->next;
            }

            free_unified_package(current);
            upm->package_count--;

            printf(COL_BRIGHT_RED "\n  ❌ Package uninstalled successfully!\n" COL_RESET);
            return true;
        }
        prev = current;
        current = current->next;
    }

    show_status_message("Package not found", "warning");
    return false;
}

bool list_packages(UnifiedPackageManager* upm) {
    if (!upm) return false;

    printf(COL_BOLD_MAGENTA "\n  📦 Installed Packages:" COL_RESET "\n");
    printf(COL_BOLD_CYAN "  ┌─────────────────────────────────────────────────────────┐\n" COL_RESET);

    UnifiedPackage* current = upm->packages;
    if (!current) {
        printf(COL_BOLD_CYAN "  │" COL_RESET " " COL_BRIGHT_YELLOW "❌ No packages installed" COL_RESET " " COL_BOLD_CYAN "                           │\n" COL_RESET);
    } else {
        int count = 0;
        while (current) {
            printf(COL_BOLD_CYAN "  │" COL_RESET " " COL_BRIGHT_GREEN "✅ %2d. %s" COL_RESET " (" COL_BOLD_YELLOW "v%s" COL_RESET ") " COL_RESET "\n",
                   ++count, current->name, current->version ? current->version : "unknown");
            current = current->next;
        }
    }

    printf(COL_BOLD_CYAN "  └─────────────────────────────────────────────────────────┘\n" COL_RESET);
    printf(COL_BRIGHT_CYAN "  📊 Total packages: " COL_BOLD_WHITE "%d\n" COL_RESET, upm->package_count);

    return true;
}

// Fungsi-fungsi manajemen proyek
bool create_project(const char* project_name) {
    if (!project_name) return false;

    // Validasi panjang nama proyek untuk mencegah buffer overflow
    size_t project_name_len = strlen(project_name);
    if (project_name_len == 0) {
        show_status_message("Project name cannot be empty", "error");
        return false;
    }

    // Maksimum panjang nama proyek agar path tetap dalam batas aman (512 byte)
    if (project_name_len > 250) {
        show_status_message("Project name too long (max 250 characters)", "error");
        return false;
    }

    printf(COL_BOLD_MAGENTA "\n  🚀 Creating new K# project: " COL_BOLD_CYAN "%s" COL_RESET "\n", project_name);

    // Animasi loading untuk pembuatan direktori
    show_loading_animation(COL_BRIGHT_YELLOW "Creating project directory" COL_RESET, 1);

    // Buat direktori project
    if (mkdir(project_name, 0755) == -1) {
        show_status_message("Could not create project directory", "error");
        return false;
    }

    // Animasi loading untuk pembuatan subdirektori
    show_loading_animation(COL_BRIGHT_YELLOW "Creating subdirectories" COL_RESET, 1);

    // Buat subdirektori
    char src_dir[512], test_dir[512];
    snprintf(src_dir, sizeof(src_dir), "%s/src", project_name);
    snprintf(test_dir, sizeof(test_dir), "%s/test", project_name);

    if (!create_directory_recursive(src_dir) || !create_directory_recursive(test_dir)) {
        show_status_message("Could not create project subdirectories", "error");
        return false;
    }

    // Animasi loading untuk pembuatan file
    show_loading_animation(COL_BRIGHT_YELLOW "Generating project files" COL_RESET, 1);

    // Buat file main.k
    char main_file[512];
    snprintf(main_file, sizeof(main_file), "%s/src/main.k", project_name);

    FILE* main_fp = fopen(main_file, "w");
    if (main_fp) {
        fprintf(main_fp, "pub fn main() {\n");
        fprintf(main_fp, "    print(\"Hello, %s!\\n\")\n", project_name);
        fprintf(main_fp, "}\n");
        fclose(main_fp);
    }

    // Buat file konfigurasi module (kmod.k)
    char config_file[512];
    snprintf(config_file, sizeof(config_file), "%s/kmod.k", project_name);

    FILE* config_fp = fopen(config_file, "w");
    if (config_fp) {
        fprintf(config_fp, "module %s\n\n", project_name);
        fprintf(config_fp, "ksharp 0.0.1\n\n");
        fprintf(config_fp, "require (\n");
        fprintf(config_fp, "    // Add your dependencies here\n");
        fprintf(config_fp, ")\n");
        fclose(config_fp);
    }

    printf(COL_BRIGHT_GREEN "\n  ✅ Project created successfully!" COL_RESET "\n");
    return true;
}

// Fungsi-fungsi manajemen metadata header-only
HeaderMeta* create_header_meta() {
    HeaderMeta* meta = malloc(sizeof(HeaderMeta));
    if (!meta) return NULL;

    meta->package_name = NULL;
    meta->version = NULL;
    meta->description = NULL;
    meta->author = NULL;
    meta->license = NULL;
    meta->dependencies = NULL;
    meta->keywords = NULL;
    meta->homepage = NULL;
    meta->repository = NULL;
    meta->bugs = NULL;
    meta->contributors = NULL;
    meta->categories = NULL;
    meta->tags = NULL;

    return meta;
}

void free_header_meta(HeaderMeta* meta) {
    if (!meta) return;

    free(meta->package_name);
    free(meta->version);
    free(meta->description);
    free(meta->author);
    free(meta->license);
    free(meta->dependencies);
    free(meta->keywords);
    free(meta->homepage);
    free(meta->repository);
    free(meta->bugs);
    free(meta->contributors);
    free(meta->categories);
    free(meta->tags);

    free(meta);
}

bool parse_header_meta_from_file(const char* filepath, HeaderMeta* meta) {
    if (!filepath || !meta) return false;

    char* content = read_file_safe(filepath);
    if (!content) return false;

    char* content_copy = strdup(content);  // Buat salinan untuk diproses
    if (!content_copy) {
        free(content);
        return false;
    }

    char* saveptr = NULL;  // Untuk penggunaan strtok_r yang aman
    char* line = strtok_r(content_copy, "\n", &saveptr);

    while (line) {
        // Periksa apakah baris adalah header K# yang valid
        if (strncmp(line, "// @", 4) == 0) {
            const char* content_ptr = line + 4; // Lewati "// @"

            // Lewati spasi awal
            while (*content_ptr && isspace(*content_ptr)) {
                content_ptr++;
            }

            // Ekstrak key-value
            const char* key_start = content_ptr;
            while (*content_ptr && !isspace(*content_ptr) && *content_ptr != ':') {
                content_ptr++;
            }

            if (content_ptr > key_start) {
                int key_len = content_ptr - key_start;
                char key[256];
                strncpy(key, key_start, key_len);
                key[key_len] = '\0';

                // Lewati delimiter
                while (*content_ptr && (isspace(*content_ptr) || *content_ptr == ':')) {
                    content_ptr++;
                }

                // Nilai adalah sisa dari baris
                const char* value_start = content_ptr;

                // Alokasikan dan simpan nilai ke field yang sesuai
                if (strcmp(key, "name") == 0 || strcmp(key, "package") == 0) {
                    free(meta->package_name);
                    meta->package_name = strdup(value_start);
                } else if (strcmp(key, "version") == 0) {
                    free(meta->version);
                    meta->version = strdup(value_start);
                } else if (strcmp(key, "description") == 0) {
                    free(meta->description);
                    meta->description = strdup(value_start);
                } else if (strcmp(key, "author") == 0) {
                    free(meta->author);
                    meta->author = strdup(value_start);
                } else if (strcmp(key, "license") == 0) {
                    free(meta->license);
                    meta->license = strdup(value_start);
                } else if (strcmp(key, "dependencies") == 0 || strcmp(key, "deps") == 0) {
                    free(meta->dependencies);
                    meta->dependencies = strdup(value_start);
                } else if (strcmp(key, "keywords") == 0) {
                    free(meta->keywords);
                    meta->keywords = strdup(value_start);
                } else if (strcmp(key, "homepage") == 0) {
                    free(meta->homepage);
                    meta->homepage = strdup(value_start);
                } else if (strcmp(key, "repository") == 0) {
                    free(meta->repository);
                    meta->repository = strdup(value_start);
                } else if (strcmp(key, "bugs") == 0) {
                    free(meta->bugs);
                    meta->bugs = strdup(value_start);
                } else if (strcmp(key, "contributors") == 0) {
                    free(meta->contributors);
                    meta->contributors = strdup(value_start);
                } else if (strcmp(key, "categories") == 0) {
                    free(meta->categories);
                    meta->categories = strdup(value_start);
                } else if (strcmp(key, "tags") == 0) {
                    free(meta->tags);
                    meta->tags = strdup(value_start);
                }
            }
        }

        line = strtok_r(NULL, "\n", &saveptr);
    }

    free(content);
    free(content_copy);
    return true;
}

// bool save_header_meta_to_file(const char* filepath, const HeaderMeta* meta) {
//     if (!filepath || !meta) return false;
//
//     // Baca konten file asli
//     char* original_content = read_file_safe(filepath);
//     if (!original_content) {
//         // Jika file tidak ada, buat file baru dengan metadata
//         FILE* fp = fopen(filepath, "w");
//         if (!fp) return false;
//
//         fprintf(fp, "// @name %s\n", meta->package_name ? meta->package_name : "unknown");
//         fprintf(fp, "// @version %s\n", meta->version ? meta->version : "0.0.1");
//         fprintf(fp, "// @description %s\n", meta->description ? meta->description : "No description");
//         fprintf(fp, "// @author %s\n", meta->author ? meta->author : "Unknown");
//         fprintf(fp, "\n");
//
//         fclose(fp);
//         return true;
//     }
//
//     // Temukan posisi header metadata (jika ada)
//     const char* header_end = strstr(original_content, "\n\n"); // Asumsikan header dipisahkan oleh double newline
//     if (!header_end) {
//         header_end = strstr(original_content, "\n"); // Atau single newline
//     }
//
//     FILE* fp = fopen(filepath, "w");
//     if (!fp) {
//         free(original_content);
//         return false;
//     }
//
//     // Tulis metadata header
//     fprintf(fp, "// @name %s\n", meta->package_name ? meta->package_name : "unknown");
//     fprintf(fp, "// @version %s\n", meta->version ? meta->version : "0.0.1");
//     fprintf(fp, "// @description %s\n", meta->description ? meta->description : "No description");
//     fprintf(fp, "// @author %s\n", meta->author ? meta->author : "Unknown");
//     if (meta->license) fprintf(fp, "// @license %s\n", meta->license);
//     if (meta->dependencies) fprintf(fp, "// @dependencies %s\n", meta->dependencies);
//     if (meta->keywords) fprintf(fp, "// @keywords %s\n", meta->keywords);
//     if (meta->homepage) fprintf(fp, "// @homepage %s\n", meta->homepage);
//     if (meta->repository) fprintf(fp, "// @repository %s\n", meta->repository);
//     if (meta->bugs) fprintf(fp, "// @bugs %s\n", meta->bugs);
//     if (meta->contributors) fprintf(fp, "// @contributors %s\n", meta->contributors);
//     if (meta->categories) fprintf(fp, "// @categories %s\n", meta->categories);
//     if (meta->tags) fprintf(fp, "// @tags %s\n", meta->tags);
//     fprintf(fp, "\n");
//
//     // Jika ada konten asli setelah header, tulis juga
//     if (header_end) {
//         fprintf(fp, "%s", header_end + 1); // Lewati newline pertama
//     } else {
//         fprintf(fp, "%s", original_content);
//     }
//
//     fclose(fp);
//     free(original_content);
//     return true;
// }

// Fungsi untuk menata dependensi berdasarkan kmod.k
bool tidy_dependencies(UnifiedPackageManager* upm) {
    if (!upm) return false;

    printf(COL_BOLD_YELLOW "\n  🧹 Tidying dependencies...\n" COL_RESET);

    // Cek apakah ada file kmod.k di direktori saat ini
    KModInfo* kmod_info = parse_kmod_file("kmod.k");
    if (!kmod_info) {
        show_status_message("No kmod.k file found in current directory", "warning");
        return false;
    }

    printf(COL_BOLD_CYAN "  Found module: %s\n" COL_RESET, kmod_info->module_name ? kmod_info->module_name : "unknown");

    if (kmod_info->req_count > 0) {
        printf(COL_BOLD_CYAN "  Found %d dependencies:\n" COL_RESET, kmod_info->req_count);

        for (int i = 0; i < kmod_info->req_count; i++) {
            printf(COL_BRIGHT_GREEN "    - %s\n" COL_RESET, kmod_info->requirements[i]);

            // Parse nama paket dan versi dari requirement
            char* req_copy = strdup(kmod_info->requirements[i]);
            char* token = strtok(req_copy, " ");
            if (token) {
                const char* package_name = token;
                const char* version = strtok(NULL, " ");

                // Cek apakah paket sudah terinstal
                bool found = false;
                UnifiedPackage* current = upm->packages;
                while (current) {
                    if (strcmp(current->name, package_name) == 0) {
                        found = true;
                        break;
                    }
                    current = current->next;
                }

                if (!found) {
                    printf(COL_BRIGHT_YELLOW "    Installing missing dependency: %s %s\n" COL_RESET, package_name, version ? version : "latest");
                    install_package(upm, package_name, version);
                } else {
                    printf(COL_BRIGHT_GREEN "    ✓ %s already installed\n" COL_RESET, package_name);
                }
            }
            free(req_copy);
        }
    } else {
        printf(COL_BRIGHT_YELLOW "  No dependencies found in kmod.k\n" COL_RESET);
    }

    free_kmod_info(kmod_info);
    show_status_message("Dependencies tidied successfully", "success");
    return true;
}






