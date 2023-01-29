#ifndef MALLOC_H
#define MALLOC_H

// Structure of a header.
typedef struct header
{
    // Address of the previous header.
    struct header* prev;

    // Address of the next header.
    struct header* next;

    // Size of the data being used.
    size_t size;

    // YES (1), if the data is free.
    // NO  (0), if the data is not free.
    size_t free;
} header;

// Size of a header in bytes.
extern const size_t HSIZE;

// Constants used for the "free" field.
extern const size_t YES;
extern const size_t NO;

// Gets the starting address of a data section.
void* get_data(header* h);

// Gets a header from a data pointer.
// The data pointer must point to a valid data section.
header* get_header_from_data(void* data);

// Gets the total number of bytes for a data section.
size_t get_total(header* h);

// Gets the address of the sentinel.
header* get_sentinel();

// Initializes the heap with the sentinel.
void init_heap();

// Expands the heap.
// last_header: Address of the last header.
// size: Number of new bytes to store in the heap.
//       This value must be a multiple of eight.
// Returns the address of the new header.
header* expand_heap(header* last_header, size_t size);

// Finds the first free chunk that can contain
// the number of bytes passed as a parameter.
// Returns the header of that chunk.
// If no chunk can be found, returns the last header.
header* find_free_chunk(size_t size);

// Allocates "size" bytes and returns a pointer to the allocated memory.
void* my_malloc(size_t size);

// Allocates memory for an array of "nmemb" elements of "size" bytes each
// and returns a pointer  to  the  allocated memory.
void* my_calloc(size_t nmemb, size_t size);

// Frees the memory space pointed to by "ptr".
// The data pointer must point to a valid data section.
void my_free(void* ptr);

// Changes the size of the memory block pointed to by "ptr" to "size" bytes.
// The data pointer must point to a valid data section.
void* my_realloc(void* ptr, size_t size);

#endif