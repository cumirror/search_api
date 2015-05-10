#include "search_generic.h"
#include <stdlib.h>
#include <string.h>

static struct Search_ops *search_objs;

static struct Search_ops *find_search_ops(char *name)
{
    struct Search_ops * so = NULL;
    for (so = search_objs; so != NULL; so = so->next) {
        if (!strcmp(so->name, name))
            return so;
    }

    return NULL;
}

int register_search_ops(struct Search_ops *ops)
{
    struct Search_ops *so = NULL;
    struct Search_ops **sop = NULL;
    for (sop = &search_objs; (so = *sop) != NULL; sop = &(so->next)) {
        if (!strcmp(so->name, ops->name))
            goto out;
    }

    ops->next = NULL;
    *sop = ops;

out:
    return 0;
}

struct Search *search_new(char *name)
{
    struct Search *s = NULL;
    struct Search_ops *ops = NULL;
    if ((ops = find_search_ops(name)) != NULL) {
        s = (struct Search*)malloc(sizeof(struct Search) + ops->priv_size);
        s->ops = ops;
        s->ops->init(s);
    }

    return s;
}




