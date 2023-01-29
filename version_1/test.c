#include <stdio.h>
#include <unistd.h>
#include "malloc.h"

void print_header(header *h)
{
    void *base = get_sentinel();

    if (h->prev == NULL)
        printf("prev          = Null\n");
    else
        printf("prev          = 0x%lx\n", (void *)h->prev - base);

    if (h->next == NULL)
        printf("next          = Null\n");
    else
        printf("next          = 0x%lx\n", (void *)h->next - base);

    printf("size (used)   = 0x%02zx (%lu)\n", h->size, h->size);

    if (h == base)
    {
        printf("size (unused) = No data\n");
        printf("size (total)  = No data\n");
        printf("data (start)  = No data\n");
        printf("data (end)    = No data\n");
    }
    else
    {
        size_t total = get_total(h);
        size_t unused = total - h->size;
        void *data = get_data(h);

        printf("size (unused) = 0x%02lx (%zu)\n", unused, unused);
        printf("size (total)  = 0x%02lx (%zu)\n", total, total);
        printf("data (start)  = 0x%lx\n", data - base);
        printf("data (end)    = 0x%lx\n", data + total - 1 - base);
    }

    if (h->free)
        printf("free          = \033[32m\033[1mYes\033[0m\n");
    else
        printf("free          = \033[31m\033[1mNo\033[0m\n");
}

void print_heap()
{
    header *sentinel = get_sentinel();
    header *end = sbrk(0);

    printf("             HEAP\n");
    printf("============================== Chunk 0 (sentinel)\n");
    print_header(sentinel);
    size_t count = 1;

    for (header *h = sentinel->next; h != end; h = h->next)
    {
        printf("------------------------------ Chunk %zu (0x%lx)\n", count++, (void *)h - (void *)sentinel);
        print_header(h);
    }

    printf("==============================\n");
}

void test_init_heap()
{
    init_heap();
    print_heap();
}

void test_my_malloc()
{
    init_heap();

    printf("my_malloc(0) = %p\n", my_malloc(0));

    my_malloc(5);
    void *p = my_malloc(32);
    my_malloc(200);
    print_heap();

    header *h = get_header_from_data(p);
    h->free = YES;
    print_heap();

    my_malloc(18);
    print_heap();
}

void test_my_calloc()
{
    init_heap();

    printf("my_calloc(0, 0) = %p\n", my_calloc(0, 0));
    printf("my_calloc(5, 0) = %p\n", my_calloc(5, 0));
    printf("my_calloc(0, 5) = %p\n", my_calloc(0, 5));
    printf("my_calloc(0x100000000, 0x100000000) = %p\n", my_calloc(0x100000000, 0x100000000));

    char *p1, *p2, *p3;
    header *h1, *h2, *h3;
    size_t i;
    p1 = my_calloc(2, 5);
    p2 = my_calloc(3, 8);
    p3 = my_calloc(100, 2);
    print_heap();

    h1 = get_header_from_data(p1);
    h2 = get_header_from_data(p2);
    h3 = get_header_from_data(p3);
    h1->free = YES;
    h2->free = YES;
    h3->free = YES;
    print_heap();

    for (i = 0; i < h1->size; i++)
        p1[i] = 0xff;
    for (i = 0; i < h2->size; i++)
        p2[i] = 0xff;
    for (i = 0; i < h3->size; i++)
        p3[i] = 0xff;
    p1 = my_calloc(1, 5);
    p2 = my_calloc(2, 8);
    p3 = my_calloc(1, 2);
    print_heap();

    for (i = 0; i < h1->size; i++)
        if (p1[i] != 0)
            break;
    if (i == h1->size)
        printf("Chunk 1 contains only zeros.\n");
    else
        printf("Chunk 1 does not contain only zeros.\n");

    for (i = 0; i < h2->size; i++)
        if (p2[i] != 0)
            break;
    if (i == h2->size)
        printf("Chunk 2 contains only zeros.\n");
    else
        printf("Chunk 2 does not contain only zeros.\n");

    for (i = 0; i < h3->size; i++)
        if (p3[i] != 0)
            break;
    if (i == h3->size)
        printf("Chunk 3 contains only zeros.\n");
    else
        printf("Chunk 3 does not contain only zeros.\n");
}

void test_my_free()
{
    init_heap();

    void *p1, *p2, *p3, *p4;
    p1 = my_malloc(10);
    p2 = my_malloc(20);
    p3 = my_calloc(2, 15);
    p4 = my_calloc(4, 10);

    my_free(p1);
    my_free(p3);
    print_heap();

    my_free(p2);
    my_free(p4);
    print_heap();
}

void test_my_realloc()
{
    init_heap();

    void *p1, *p2;
    p1 = my_realloc(NULL, 5);
    p2 = my_realloc(NULL, 70);
    print_heap();
    p1 = my_realloc(p1, 7);
    print_heap();
    p1 = my_realloc(p1, 19);
    print_heap();

    my_realloc(p1, 0);
    my_realloc(p2, 0);
}

int main()
{
    // Do not remove this first instruction
    // because the first call to printf may change the program break.
    printf("Tests\n");

    // Uncomment only one test function at a time:
    // test_init_heap();
    // test_my_malloc();
    // test_my_calloc();
    // test_my_free();
    test_my_realloc();

    return 0;
}