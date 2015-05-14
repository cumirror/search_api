/*
 * http://en.wikipedia.org/wiki/Boyer–Moore_string_search_algorithm
 */
#include "search_generic.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define ALPHABET_LEN 256
#define BM_MAX_PATTERN_SIZE 16

struct bm_search_data {
    int delta1[ALPHABET_LEN];
    int delta2[BM_MAX_PATTERN_SIZE];
    char pattern[BM_MAX_PATTERN_SIZE];
    int len;
};

#define NOT_FOUND patlen
#define max(a, b) ((a < b) ? b : a)

// delta1 table: delta1[c] contains the distance between the last
// character of pat and the rightmost occurrence of c in pat.
// If c does not occur in pat, then delta1[c] = patlen.
// If c is at string[i] and c != pat[patlen-1], we can
// safely shift i over by delta1[c], which is the minimum distance
// needed to shift pat forward to get string[i] lined up
// with some character in pat.
// this algorithm runs in alphabet_len+patlen time.
static void make_delta1(int *delta1, char *pat, int patlen)
{
    int i;
    for (i=0; i < ALPHABET_LEN; i++) {
        delta1[i] = NOT_FOUND;
    }
    for (i=0; i < patlen-1; i++) {
        delta1[pat[i]] = patlen - 1 - i;
    }
}

// true if the suffix of word starting from word[pos] is a prefix
// of word
static int is_prefix(char *word, int wordlen, int pos)
{
    int i;
    int suffixlen = wordlen - pos;
    // could also use the strncmp() library function here
    for (i = 0; i < suffixlen; i++) {
        if (word[i] != word[pos+i]) {
            return 0;
        }
    }
    return 1;
}

// length of the longest suffix of word ending on word[pos].
// suffix_length("dddbcabc", 8, 4) = 2
static int suffix_length(char *word, int wordlen, int pos)
{
    int i;
    // increment suffix length i to the first mismatch or beginning
    // of the word
    for (i = 0; (word[pos-i] == word[wordlen-1-i]) && (i < pos); i++);
    return i;
}

// delta2 table: given a mismatch at pat[pos], we want to align
// with the next possible full match could be based on what we
// know about pat[pos+1] to pat[patlen-1].
//
// In case 1:
// pat[pos+1] to pat[patlen-1] does not occur elsewhere in pat,
// the next plausible match starts at or after the mismatch.
// If, within the substring pat[pos+1 .. patlen-1], lies a prefix
// of pat, the next plausible match is here (if there are multiple
// prefixes in the substring, pick the longest). Otherwise, the
// next plausible match starts past the character aligned with
// pat[patlen-1].
//
// In case 2:
// pat[pos+1] to pat[patlen-1] does occur elsewhere in pat. The
// mismatch tells us that we are not looking at the end of a match.
// We may, however, be looking at the middle of a match.
//
// The first loop, which takes care of case 1, is analogous to
// the KMP table, adapted for a 'backwards' scan order with the
// additional restriction that the substrings it considers as
// potential prefixes are all suffixes. In the worst case scenario
// pat consists of the same letter repeated, so every suffix is
// a prefix. This loop alone is not sufficient, however:
// Suppose that pat is "ABYXCDBYX", and text is ".....ABYXCDEYX".
// We will match X, Y, and find B != E. There is no prefix of pat
// in the suffix "YX", so the first loop tells us to skip forward
// by 9 characters.
// Although superficially similar to the KMP table, the KMP table
// relies on information about the beginning of the partial match
// that the BM algorithm does not have.
//
// The second loop addresses case 2. Since suffix_length may not be
// unique, we want to take the minimum value, which will tell us
// how far away the closest potential match is.
static void make_delta2(int *delta2, char *pat, int patlen)
{
    int p;
    int last_prefix_index = patlen-1;

    // first loop
    for (p=patlen-1; p>=0; p--) {
        if (is_prefix(pat, patlen, p+1)) {
            last_prefix_index = p+1;
        }
        delta2[p] = last_prefix_index + (patlen-1 - p);
    }

    // second loop
    for (p=0; p < patlen-1; p++) {
        int slen = suffix_length(pat, patlen, p);
        if (pat[p - slen] != pat[patlen-1 - slen]) {
            delta2[patlen-1 - slen] = patlen-1 - p + slen;
        }
    }
}

static int bm_init(struct Search *s_obj)
{
    struct bm_search_data *sd = (struct bm_search_data *)(s_obj->priv);

    bzero(sd, sizeof(*sd));
    return 0;
}

static int bm_add(struct Search *s_obj, char *pattern, int len)
{
    struct bm_search_data *sd = (struct bm_search_data *)(s_obj->priv);

    if (len >= BM_MAX_PATTERN_SIZE)
        return -1;

    memcpy(sd->pattern, pattern, len);
    sd->len = len;

    return 0;
}

static int bm_build(struct Search *s_obj)
{
    struct bm_search_data *sd = (struct bm_search_data *)(s_obj->priv);

    make_delta1(sd->delta1, sd->pattern, sd->len);
    make_delta2(sd->delta2, sd->pattern, sd->len);

    return 0;
}

static int bm_search(struct Search *s_obj, char *text, int n)
{
    struct bm_search_data *sd = (struct bm_search_data *)(s_obj->priv);
    int patlen = sd->len;
    char *pat = sd->pattern;
    int *delta1 = sd->delta1;
    int *delta2 = sd->delta2;
    char *ntext = text;
    int i, j, nn;

   i = patlen - 1;
   nn = n;
    while (i < nn) {
        int j = patlen - 1;
        while (j >= 0 && (ntext[i] == pat[j])) {
            --i;
            --j;
        }
        if (j < 0) {
            printf("bm: found at %d\n", i + 1 + (n - nn));
            nn -= i + 1 + 1;
            ntext += i + 1 + 1;
            i = patlen - 1;
        } else
        i += max(delta1[ntext[i]], delta2[j]);
    }

    return 0;
}

static int bm_dump(struct Search *s_obj)
{
    struct bm_search_data *sd = (struct bm_search_data *)(s_obj->priv);
    int patlen = sd->len;
    char *pat = sd->pattern;
    int *delta1 = sd->delta1;
    int *delta2 = sd->delta2;
    int i;

    printf("Boyer-Moore: Search pattern %s", pat);
    printf("\n\tALPHABET Table:\n\t");
    for (i=0; i < ALPHABET_LEN; i++) {
        if (delta1[i] != NOT_FOUND)
            printf("%2c  ", i);
    }
    printf("\n\tSKIP Table:\n\t");
    for (i=0; i < patlen; i++) {
        printf("%2d  ", delta2[i]);
    }
    printf("\n");

    return 0;
}

static int bm_destroy(struct Search *s_obj)
{
    /* do nothing */
    free(s_obj);
    return 0;
}

static struct Search_ops bm_search_ops = {
    .name = "bm",
    .priv_size = sizeof(struct bm_search_data),
    .init = bm_init,
    .add = bm_add,
    .build = bm_build,
    .search = bm_search,
    .dump = bm_dump,
    .destroy = bm_destroy,
};

int bm_module_init()
{
    return register_search_ops(&bm_search_ops);
}
