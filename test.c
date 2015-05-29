#include "search_generic.h"
#include <stdio.h>
#include <string.h>

extern int kmp_module_init();
extern int bm_module_init();
extern int ac_module_init();

int kmp_test()
{
    struct Search * s = search_new("kmp");
    //char *pattern  = "GCAGAGAG";
    //char *text  = "sfdAGCAGAGAGGCAGAGAGBsABCDABDdfCdsABCDABDfDABDABCDABD";
    char *pattern  = "ababa";
    char *text  = "sfdAGCAGAGAGGCabababababAGAGAGBsABCDABDdfCdsABCDABDfDABDABCDABD";

    search_add(s, pattern, strlen(pattern), 0);
    search_build(s);
    search_dump(s);
    search_now(s, text, strlen(text));
    return 0;
}

int bm_test()
{
    struct Search * s = search_new("bm");
    //char *pattern  = "EXAMPLE";
    //char *text  = "HERE IS A SIMPLE EXAMPLE";
    char *pattern  = "ababa";
    char *text  = "sfdAGCAGAGAGGCabababababAGAGAGBsABCDABDdfCdsABCDABDfDABDABCDABD";

    search_add(s, pattern, strlen(pattern), 0);
    search_build(s);
    search_dump(s);
    search_now(s, text, strlen(text));
    return 0;
}

int ac_test()
{
    struct Search * s = search_new("ac");
    char *pattern1  = "ababa";
    char *pattern2 = "ds";
    char *pattern3  = "ABD";
    char *text  = "sfdAGCAGAGAGGCabababababAGAGAGBsABCDABDdfCdsABCDABDfDABDABCDABD";

    search_add(s, pattern1, strlen(pattern1), 1);
    search_add(s, pattern2, strlen(pattern2), 5);
    search_add(s, pattern3, strlen(pattern3), 6);
    search_build(s);
    search_dump(s);
    search_now(s, text, strlen(text));
    return 0;
}


int main()
{
    kmp_module_init();
    kmp_test();

    bm_module_init();
    bm_test();

    ac_module_init();
    ac_test();

    return 0;
}
