#include "search_generic.h"
#include <stdio.h>
#include <string.h>

extern int kmp_module_init();

int kmp_test()
{
    struct Search * s = search_new("kmp");
    //char *pattern  = "GCAGAGAG";
    //char *text  = "sfdAGCAGAGAGGCAGAGAGBsABCDABDdfCdsABCDABDfDABDABCDABD";
    char *pattern  = "ababa";
    char *text  = "sfdAGCAGAGAGGCabababababAGAGAGBsABCDABDdfCdsABCDABDfDABDABCDABD";

    search_add(s, pattern, strlen(pattern));
    search_build(s);
    search_dump(s);
    search_now(s, text, strlen(text));
    return 0;
}

int main()
{
    kmp_module_init();

    kmp_test();

    return 0;
}
