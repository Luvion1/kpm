#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/pkg/kmod_parser.h"
#include "../src/util/utils.h"

void test_parse_kmod() {
    printf("Testing kmod parser... ");

    // Create a dummy kmod file
    FILE* fp = fopen("test_kmod.k", "w");
    fprintf(fp, "module github.com/test/repo\n");
    fprintf(fp, "ksharp 0.1.0\n");
    fprintf(fp, "require (\n");
    fprintf(fp, "    github.com/test/dep 1.0.0\n");
    fprintf(fp, ")\n");
    fclose(fp);

    KModInfo* info = parse_kmod_file("test_kmod.k");

    assert(info != NULL);
    assert(strcmp(info->module_name, "github.com/test/repo") == 0);
    assert(strcmp(info->ksharp_version, "0.1.0") == 0);
    assert(info->req_count == 1);
    assert(strstr(info->requirements[0], "github.com/test/dep") != NULL);

    free_kmod_info(info);
    remove("test_kmod.k");
    printf("PASSED\n");
}

void test_path_safety() {
    printf("Testing path safety... ");

    // Safe paths
    assert(is_path_safe("safe/path") == true);
    assert(is_path_safe("github.com/user/repo") == true);

    // Unsafe paths
    assert(is_path_safe("../escape") == false);
    assert(is_path_safe("path/with<danger>") == false);
    assert(is_path_safe("path|with|pipe") == false);

    printf("PASSED\n");
}

void test_read_file_bounds() {
    printf("Testing file reading bounds... ");

    // Create a large test file
    FILE* fp = fopen("large_test.txt", "w");
    for (int i = 0; i < 10000; i++) {
        fprintf(fp, "This is line %d with some content to make it larger.\n", i);
    }
    fclose(fp);

    char* content = read_file("large_test.txt");
    assert(content != NULL);
    free(content);
    remove("large_test.txt");

    printf("PASSED\n");
}

int main() {
    test_parse_kmod();
    test_path_safety();
    test_read_file_bounds();
    printf("All tests passed!\n");
    return 0;
}
