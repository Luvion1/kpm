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

int main() {
    test_parse_kmod();
    printf("All tests passed!\n");
    return 0;
}
