#include <unistd.h>
#include <string.h>
#include <err.h>
#include "malloc.h"

#include <stdio.h>

const size_t HSIZE = sizeof(header);
const size_t YES = 1;
const size_t NO = 0;

static header* _sentinel = NULL;

void* get_data(header* h)
{
    // Option:
    //   return (void*)((size_t)h + HSIZE);
    return (void*)(h + 1);
}

header* get_header_from_data(void* data)
{
    // Option:
    //   return (header*)((size_t)data - HSIZE);
    return ((header*)data) - 1;
}

size_t get_total(header* h)
{
    // DEBUG:
    //   printf("h\t: %p\n", h);
    //   printf("h->next\t: %p\n", h->next);
    //   printf("h->prev\t: %p\n", h->prev);
    //   printf("h+HSIZE\t: %p\n", h+HSIZE);
    return (size_t)h->next - ((size_t)h + HSIZE);
}

header* get_sentinel()
{
    // TODO:
    // - Define and initialize a static variable to NULL.
    // - If this variable is NULL,
    //   it is the first call to the function.
    //   So, set it to the initial address of the heap
    //   (the initial value of the program break).
    // - If this variable is not NULL,
    //   it contains the initial value of the heap,
    //   which is also the address of the sentinel.
    //   So, just return it.
    if (_sentinel != NULL) {
        return _sentinel;
    }
    _sentinel = (header*) sbrk(0);
    sbrk(HSIZE);
    _sentinel->prev = NULL;
    _sentinel->next = sbrk(0);
    _sentinel->size = 0;
    _sentinel->free = NO;
}

void init_heap()
{
    // TODO:
    // - Get the address of the sentinel
    // - Allocate the memory for the sentinel
    //   (obviously do not use malloc).
    // - Initialize the sentinel with the following values:
    //   - prev = NULL
    //   - next = current program break
    //   - size = 0
    //   - free = No
    _sentinel = NULL; // TODO: Free the memory allocated for the sentinel.
    get_sentinel();
}

header* expand_heap(header* last_header, size_t size)
{
    // TODO:
    // - Get the current program break.
    // - Expand the heap
    //   (Allocate the memory for the
    //    new header and the data section.)
    //   If an error occurs, return NULL.
    // - Initialize the next and prev fields of the new header.
    // - Update the previous header.
    // - Return the new header.
    // NOTE:
    // - This function have to be called only
    //   when the value of the last_header is same as the program break.
    size_t size_multiple_of_8 = size;
    if (size % 8 != 0) {
        size_multiple_of_8 = (size / 8 + 1) * 8;
    }
    sbrk(HSIZE + size_multiple_of_8);
    header* new_header = last_header->next;
    new_header->prev = last_header;
    new_header->next = sbrk(0);
    new_header->size = size;
    new_header->free = NO;
    last_header->next = new_header;
    return new_header;
}

int _is_allocatable(header* header, size_t size)
{
    if (header == _sentinel) {
        return 0;
    }    
    if (header->free == NO) {
        return 0;
    }
    if (get_total(header) < size) {
        return 0;
    }
    return 1;
}

header* find_free_chunk(size_t size)
{
    // TODO:
    // - Iterate over the chunks and return
    //   the first chunk (the address of the header)
    //   that is free and large enough to store "size" bytes
    //   (compare the total number of bytes with "size").
    // - If no chunk can be found,
    //   return the address of the last header.
    header* last_header = get_sentinel();
    while (last_header->next != sbrk(0)) {
        if (_is_allocatable(last_header, size)) {
            return last_header;
        }
        last_header = last_header->next;
    }
    return last_header;
}

void* my_malloc(size_t size)
{
    // TODO:
    // - If "size" is 0, return NULL.
    // - "size" must be a multiple of eight.
    //   If it is not, work out the next multiple of eight
    //   and use this value to find a free chunk large enough.
    // - Get the first free chunk large enough
    //   (or the last chunk).
    // - If no chunk can be found, expand the heap.
    // - Do not forget to update the "size" and the "free" fields. ✅
    //   Be careful, the "size" field of the header
    //   must be initialized with the "size" parameter ✅
    //   (not with the next multiple of eight). ✅
    // - Return the address of the data section. ✅
    if (size == 0) {
        return NULL;
    }
    header* found_header = find_free_chunk(size);
    // Check the found_header is free or not.
    // Because the found_header is the last chunk, 
    // that chunk could be usable.
    // We have to decide whether expand the heap or not.
    // So, check one more time.
    if(_is_allocatable(found_header, size)) {
        found_header->size = size;
        found_header->free = NO;
        return get_data(found_header);
    }
    header* last_header = expand_heap(found_header, size);
    return get_data(last_header);
}

void* my_calloc(size_t nmemb, size_t size)
{
    // TODO:
    // - Work out the size in bytes. ✅
    //   To do so, you have to multiply "nmemb" and "size". ✅
    //   Use __builtin_mul_overflow() to detect an overflow. ✅
    //   If an overflow occurs, return NULL. ✅
    // - Call my_malloc() and return NULL if an error occurs. ✅
    // - Fill the memory space with zeros (use memset(3)). ✅
    // - Return the address of the data section. ✅
    size_t to_allocate_byte = 0;
    if (__builtin_mul_overflow(nmemb, size, &to_allocate_byte)) {
        return NULL; // Overflow
    }
    // to_allocate_byte is the size in bytes (nmemb * size).
    void* data_section = my_malloc(to_allocate_byte);
    if (data_section == NULL) {
        return NULL;
    }
    memset(data_section, 0, size);
    return data_section;
}

void my_free(void* ptr)
{
    // TODO:
    // - If "ptr" is NULL, no operation is performed. ✅
    // - Get the header. ✅
    // - Mark the chunk as free. ✅
    if (ptr == NULL) {
        return;
    }
    header* header = get_header_from_data(ptr);
    header->free = YES;
}

void* my_realloc(void* ptr, size_t size)
{
    // TODO:
    // - If "ptr" is NULL, realloc() is equivalent to malloc(). ✅
    // - If "size" is 0 (and "ptr" is not NULL), ✅
    //   realloc() is equivalent to free() and return NULL. ✅
    // - Get the header. ✅
    // - If this chunk is large enough, just update the size. ✅
    // - Otherwise:
    //   - Allocate a new memory space with my_malloc()
    //     (return NULL if an error occurs). ✅
    //   - Copy the bytes from the previous memory
    //     space to the new one (use memcpy(3)). ✅
    //   - Free the previous memory space. ✅
    //   - Return the address of the new memory space. ✅
    if (ptr == NULL) {
        return my_malloc(size);
    }
    if (size == 0) {
        my_free(ptr);
        return NULL;
    }
    header* header = get_header_from_data(ptr);
    if (get_total(header) >= size) {
        header->size = size;
        return ptr;
    }
    void* new_data_section = my_malloc(size);
    if (new_data_section == NULL) {
        return NULL;
    }
    memcpy(new_data_section, ptr, header->size);
    my_free(ptr);
    return new_data_section;
}