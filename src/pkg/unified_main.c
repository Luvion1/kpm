#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unified_package_manager.h"

void print_banner() {
    print_logo();
    printf(COL_BOLD "\n KSharp Package Manager (kpm)" COL_RESET "\n");
    printf(" ------------------------------\n");
    printf(" Version     : " COL_CYAN "0.0.1" COL_RESET "\n");
    printf(" OS          : " COL_CYAN "linux" COL_RESET "\n");
    printf(" Strategy    : " COL_CYAN "Go-Style (Decentralized)" COL_RESET "\n");
    printf(" Github      : " COL_CYAN "https://github.com/ksharp-lang/kpm" COL_RESET "\n");
    printf(" ------------------------------\n\n");
}

void print_usage() {
    print_banner();
    printf("Usage: " COL_BOLD "kpm" COL_RESET " <command> [arguments]\n\n");
    printf(COL_CYAN COL_BOLD "Commands:" COL_RESET "\n");
    printf("  init <name>                Initialize new K# module\n");
    printf("  get <url> [version]        Download and install package (e.g. github.com/user/repo)\n");
    printf("  list                       List installed packages\n");
    printf("  remove <url>               Remove a package\n");
    printf("  tidy                       Add missing and remove unused modules\n");
    
    printf("\n" COL_CYAN "Aliases:" COL_RESET "\n");
    printf("  install -> get\n");
    printf("  uninstall -> remove\n");
    printf("  resolve -> tidy\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    const char* command = argv[1];
    UnifiedPackageManager* upm = NULL;

    // Commands yang tidak memerlukan inisialisasi package manager
    if (strcmp(command, "init") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Usage: kpm init <project_name>\n");
            return 1;
        }
        print_logo();
        return create_project(argv[2]) ? 0 : 1;
    }
    // Metadata commands
    else if (strcmp(command, "meta") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: kpm meta <subcommand> [args]\n");
            return 1;
        }

        const char* subcmd = argv[2];

        if (strcmp(subcmd, "parse") == 0 || strcmp(subcmd, "show") == 0) {
            if (argc != 4) {
                fprintf(stderr, "Usage: kpm meta parse|show <file>\n");
                return 1;
            }

            HeaderMeta* meta = create_header_meta();
            if (parse_header_meta_from_file(argv[3], meta)) {
                printf("\nMetadata from: %s\n", argv[3]);
                if (meta->package_name) printf("  Name: %s\n", meta->package_name);
                if (meta->version) printf("  Version: %s\n", meta->version);
                if (meta->description) printf("  Description: %s\n", meta->description);
            }
            free_header_meta(meta);
        }
    }
    else {
        // Inisialisasi package manager untuk commands lainnya
        upm = init_unified_package_manager(NULL, NULL);
        if (!upm) return 1;

        int ret = 0;

        if (strcmp(command, "get") == 0 || strcmp(command, "install") == 0) {
            if (argc < 3) {
                fprintf(stderr, "Usage: kpm get <package> [version]\n");
                ret = 1;
            } else {
                const char* version = (argc > 3) ? argv[3] : NULL;
                if (!install_package(upm, argv[2], version)) ret = 1;
            }
        }
        else if (strcmp(command, "remove") == 0 || strcmp(command, "uninstall") == 0 || strcmp(command, "rm") == 0) {
            if (argc != 3) {
                fprintf(stderr, "Usage: kpm remove <package>\n");
                ret = 1;
            } else {
                if (!uninstall_package(upm, argv[2])) ret = 1;
            }
        }
        else if (strcmp(command, "list") == 0 || strcmp(command, "ls") == 0) {
            list_packages(upm);
        }
        else if (strcmp(command, "tidy") == 0 || strcmp(command, "resolve") == 0) {
            return tidy_dependencies(upm) ? 0 : 1;
        }
        else if (strcmp(command, "help") == 0 || strcmp(command, "--help") == 0) {
            print_usage();
        }
        else {
            print_usage();
            ret = 1;
        }

        free_unified_package_manager(upm);
        return ret;
    }

    return 0;
}
