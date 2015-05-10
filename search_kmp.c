/*
 * http://en.wikipedia.org/wiki/Knuth–Morris–Pratt_algorithm
 */
#include "search_generic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KMP_MAX_PATTERN_SIZE 16

struct kmp_search_data {
    int next[KMP_MAX_PATTERN_SIZE];
    char pattern[KMP_MAX_PATTERN_SIZE];
    int len;
};

static int kmp_init(struct Search *s_obj)
{
    struct kmp_search_data *sd = (struct kmp_search_data *)(s_obj->priv);

    bzero(sd, sizeof(*sd));
    return 0;
}

static int kmp_add(struct Search *s_obj, char *pattern, int len)
{
    struct kmp_search_data *sd = (struct kmp_search_data *)(s_obj->priv);

    if (len >= KMP_MAX_PATTERN_SIZE)
        return -1;

    memcpy(sd->pattern, pattern, len);
    sd->len = len;

    return 0;
}

#if 0
/*
 * 如果要匹配所有的match串，下面的构造并不适合
 * 如：
 * Text: AAAABABAABABAAA
 * Patern: ABABA
 * Next为：-1 0 -1 0 -1 3才正确，而下面的构造为：-1 0 -1 0 -1
 * */
static int kmp_build(struct Search *s_obj)
{
    struct kmp_search_data *sd = (struct kmp_search_data *)(s_obj->priv);
    int *next = sd->next;
    char *p = sd->pattern;
    int pLen = sd->len;
    int j, k;

    j = 0;
    k = next[0] = -1;
    while (j < pLen - 1) {
        //p[k]表示前缀，p[j]表示后缀
        if (k == -1 || p[j] == p[k]) {
            ++j;
            ++k;
            //较之前next数组求法，改动在下面4行
            if (p[j] != p[k])
                next[j] = k;   //之前只有这一行
            else
                //因为不能出现p[j] = p[ next[j ]]，所以当出现时需要继续递归，k = next[k] = next[next[k]]
                next[j] = next[k];
        } else {
            k = next[k];
        }
    }

    return 0;
}
#endif

/*
 * the length of Next array should be len + 1
 * */
static int kmp_build(struct Search *s_obj)
{
    struct kmp_search_data *sd = (struct kmp_search_data *)(s_obj->priv);
    int *next = sd->next;
    char *x = sd->pattern;
    int m = sd->len;
    int i, j;

   i = 0;
   j = next[0] = -1;
   while (i < m) {
      while (j > -1 && x[i] != x[j])
         j = next[j];
      i++;
      j++;
      if (x[i] == x[j])
         next[i] = next[j];
      else
         next[i] = j;
   }
    return 0;
}

static int kmp_search(struct Search *s_obj, char *text, int n)
{
    struct kmp_search_data *sd = (struct kmp_search_data *)(s_obj->priv);
    int i, j;
    int *next = sd->next;
    char *x = sd->pattern;
    int m = sd->len;

    /* Searching */
    i = j = 0;
    while (j < n) {
        while (i > -1 && x[i] != text[j])
            i = next[i];
        i++;
        j++;
        if (i == m) {
            printf("kmp: found at %d\n", j - i);
            i = next[i];
        }
    }

    return 0;
}

static int kmp_dump(struct Search *s_obj)
{
    struct kmp_search_data *sd = (struct kmp_search_data *)(s_obj->priv);
    int *next = sd->next;
    char *x = sd->pattern;
    int m = sd->len;
    int i;

    printf("KMP: Search pattern %s\n", x);
    printf("Next Table:\n\t");
    for(i = 0; i <= m; i++) {
        printf("%2d  ", next[i]);
    }
    printf("\n");

    return 0;
}

static int kmp_destroy(struct Search *s_obj)
{
    /* do nothing */
    free(s_obj);
    return 0;
}

static struct Search_ops kmp_search_ops = {
    .name = "kmp",
    .priv_size = sizeof(struct kmp_search_data),
    .init = kmp_init,
    .add = kmp_add,
    .build = kmp_build,
    .search = kmp_search,
    .dump = kmp_dump,
    .destroy = kmp_destroy,
};

int kmp_module_init()
{
    return register_search_ops(&kmp_search_ops);
}
