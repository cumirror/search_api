#define SEARCHNAMESIZE 8

struct Search {
    struct Search_ops *ops;
    void *priv[0];
};

struct Search_ops {
    struct Search_ops *next;
    char name[SEARCHNAMESIZE];
    int priv_size;

    int (*init)(struct Search *s_obj);
    int (*add)(struct Search *s_obj, char *pattern, int len);
    int (*build)(struct Search *s_obj);
    int (*search)(struct Search *s_obj, char *text, int len);
    int (*dump)(struct Search *s_obj);
    int (*destroy)(struct Search *s_obj);

};

int register_search_ops(struct Search_ops *ops);

struct Search *search_new(char *name);

static inline int search_add(struct Search *s_obj, char *pattern, int len)
{
    return s_obj->ops->add(s_obj, pattern, len);
}

static inline int search_build(struct Search *s_obj)
{
    return s_obj->ops->build(s_obj);
}

static inline int search_dump(struct Search *s_obj)
{
    return s_obj->ops->dump(s_obj);
}

static inline void search_destroy(struct Search *s_obj)
{
    s_obj->ops->destroy(s_obj);
}

static inline int search_now(struct Search *s_obj, char *text, int len)
{
    return s_obj->ops->search(s_obj, text, len);
}

