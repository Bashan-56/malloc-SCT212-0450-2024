
#define _DEFAULT_SOURCE

#include <unistd.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "myalloc.h"

#define ALIGNMENT 16
#define MAGIC 0x1234ABCD

typedef struct block {
    size_t size;
    struct block *next;
    int free;
    int magic;
} block_t;

#define HDR ((sizeof(block_t) + ALIGNMENT - 1) & ~(size_t)(ALIGNMENT - 1))

static block_t *head = NULL;
static block_t *tail = NULL;
static size_t total_heap_bytes = 0;
static fit_policy_t allocation_policy = FIT_FIRST;

/* Round a size up to a multiple of 16 */
static size_t align_up(size_t n)
{
    return (n + ALIGNMENT - 1) & ~(size_t)(ALIGNMENT - 1);
}

/* Get the payload address */
static void *payload(block_t *block)
{
    return (char *)block + HDR;
}

/* Get the block header from a payload pointer */
static block_t *header(void *ptr)
{
    return (block_t *)((char *)ptr - HDR);
}

/* Write all bytes to a file descriptor */
static void write_all(int fd, const char *message, size_t length)
{
    while (length > 0) {
        ssize_t written = write(fd, message, length);

        if (written <= 0) {
            return;
        }

        message += written;
        length -= (size_t)written;
    }
}

static void report_error(const char *message)
{
    write_all(STDERR_FILENO, message, strlen(message));
}

/* Find a block belonging to this allocator */
static block_t *find_block(void *ptr)
{
    block_t *current = head;

    while (current != NULL) {
        if (payload(current) == ptr) {
            return current;
        }

        current = current->next;
    }

    return NULL;
}

/* Request more memory from the operating system */
static block_t *grow(size_t need)
{
    size_t chunk_size = 65536;

    if (need > SIZE_MAX - HDR) {
        return NULL;
    }

    if (need > chunk_size - HDR) {
        chunk_size = need + HDR;
    }

    void *current_break = sbrk(0);

    if (current_break == (void *)-1) {
        return NULL;
    }

    uintptr_t address = (uintptr_t)current_break;

    size_t padding =
        (ALIGNMENT - (address % ALIGNMENT)) % ALIGNMENT;

    if (chunk_size > SIZE_MAX - padding) {
        return NULL;
    }

    size_t total_size = padding + chunk_size;

    if (total_size > (size_t)INTPTR_MAX ||
        total_heap_bytes > SIZE_MAX - total_size) {
        return NULL;
    }

    void *memory = sbrk((intptr_t)total_size);

    if (memory == (void *)-1) {
        return NULL;
    }

    block_t *new_block =
        (block_t *)((char *)memory + padding);

    new_block->size = chunk_size - HDR;
    new_block->next = NULL;
    new_block->free = 1;
    new_block->magic = MAGIC;

    if (head == NULL) {
        head = new_block;
        tail = new_block;
    } else {
        tail->next = new_block;
        tail = new_block;
    }

    total_heap_bytes += total_size;

    return new_block;
}

/* Find a suitable free block */
static block_t *find_fit(size_t need)
{
    block_t *current = head;
    block_t *best = NULL;

    while (current != NULL) {
        if (current->free && current->size >= need) {

            if (allocation_policy == FIT_FIRST) {
                return current;
            }

            if (best == NULL || current->size < best->size) {
                best = current;
            }
        }

        current = current->next;
    }

    return best;
}

/* Split a block when enough space remains */
static void split_block(block_t *block, size_t need)
{
    if (block->size < need ||
        block->size - need < HDR + ALIGNMENT) {
        return;
    }

    block_t *remainder =
        (block_t *)((char *)payload(block) + need);

    remainder->size = block->size - need - HDR;
    remainder->next = block->next;
    remainder->free = 1;
    remainder->magic = MAGIC;

    block->size = need;
    block->next = remainder;

    if (tail == block) {
        tail = remainder;
    }
}

/* Check whether two blocks are physically adjacent */
static int physically_adjacent(block_t *first, block_t *second)
{
    return (uintptr_t)payload(first) + first->size ==
           (uintptr_t)second;
}

/* Merge neighbouring free blocks */
static void coalesce(void)
{
    block_t *current = head;

    while (current != NULL && current->next != NULL) {
        block_t *next = current->next;

        if (current->free && next->free &&
            physically_adjacent(current, next)) {

            current->size += HDR + next->size;
            current->next = next->next;
            next->magic = 0;

            if (tail == next) {
                tail = current;
            }
        } else {
            current = current->next;
        }
    }
}

/* Allocate memory */
void *my_malloc(size_t size)
{
    if (size == 0 || size > SIZE_MAX - (ALIGNMENT - 1)) {
        return NULL;
    }

    size_t need = align_up(size);

    block_t *block = find_fit(need);

    if (block == NULL) {
        block = grow(need);
    }

    if (block == NULL) {
        return NULL;
    }

    split_block(block, need);
    block->free = 0;

    return payload(block);
}

/* Release allocated memory */
void my_free(void *ptr)
{
    if (ptr == NULL) {
        return;
    }

    block_t *current = find_block(ptr);

    if (current == NULL) {
        report_error("my_free: invalid pointer\n");
        return;
    }

    if (current->magic != MAGIC) {
        report_error("my_free: invalid block header\n");
        return;
    }

    if (current->free) {
        report_error("my_free: double free detected\n");
        return;
    }

    current->free = 1;
    coalesce();
}

/* Allocate zero-initialized memory */
void *my_calloc(size_t nmemb, size_t size)
{
    if (nmemb == 0 || size == 0) {
        return NULL;
    }

    if (nmemb > SIZE_MAX / size) {
        return NULL;
    }

    size_t total = nmemb * size;

    void *ptr = my_malloc(total);

    if (ptr == NULL) {
        return NULL;
    }

    memset(ptr, 0, total);

    return ptr;
}

/* Resize allocated memory */
void *my_realloc(void *ptr, size_t size)
{
    if (ptr == NULL) {
        return my_malloc(size);
    }

    if (size == 0) {
        my_free(ptr);
        return NULL;
    }

    if (size > SIZE_MAX - (ALIGNMENT - 1)) {
        return NULL;
    }

    block_t *block = find_block(ptr);

    if (block == NULL || header(ptr) != block ||
        block->magic != MAGIC || block->free) {
        report_error("my_realloc: invalid pointer\n");
        return NULL;
    }

    size_t old_size = block->size;
    size_t need = align_up(size);

    /* Shrink the existing allocation */
    if (need <= old_size) {
        split_block(block, need);
        coalesce();
        return ptr;
    }

    /* Try to expand into the next free block */
    if (block->next != NULL &&
        block->next->free &&
        physically_adjacent(block, block->next)) {

        block_t *next = block->next;

        if (old_size <= SIZE_MAX - HDR - next->size &&
            old_size + HDR + next->size >= need) {

            block->size += HDR + next->size;
            block->next = next->next;
            next->magic = 0;

            if (tail == next) {
                tail = block;
            }

            split_block(block, need);

            return ptr;
        }
    }

    /* Allocate elsewhere and copy existing contents */
    void *new_ptr = my_malloc(size);

    if (new_ptr == NULL) {
        return NULL;
    }

    memcpy(new_ptr, ptr, old_size);
    my_free(ptr);

    return new_ptr;
}

/* Select first-fit or best-fit allocation */
void set_policy(fit_policy_t p)
{
    if (p == FIT_FIRST || p == FIT_BEST) {
        allocation_policy = p;
    }
}

/* Collect heap statistics */
void heap_stats(heap_stats_t *out)
{
    if (out == NULL) {
        return;
    }

    out->heap_bytes = total_heap_bytes;
    out->used_bytes = 0;
    out->free_bytes = 0;
    out->blocks = 0;
    out->free_blocks = 0;
    out->largest_free = 0;

    block_t *current = head;

    while (current != NULL) {
        out->blocks++;

        if (current->free) {
            out->free_bytes += current->size;
            out->free_blocks++;

            if (current->size > out->largest_free) {
                out->largest_free = current->size;
            }
        } else {
            out->used_bytes += current->size;
        }

        current = current->next;
    }
}

/* Display the heap layout without printf */
void heap_dump(void)
{
    char buffer[160];

    int length = snprintf(
        buffer, sizeof(buffer),
        "Heap: %zu bytes\n", total_heap_bytes
    );

    if (length > 0) {
        size_t amount = (size_t)length;

        if (amount >= sizeof(buffer)) {
            amount = sizeof(buffer) - 1;
        }

        write_all(STDOUT_FILENO, buffer, amount);
    }

    block_t *current = head;
    size_t index = 0;

    while (current != NULL) {
        length = snprintf(
            buffer, sizeof(buffer),
            "Block %zu: address=%p size=%zu status=%s\n",
            index,
            (void *)current,
            current->size,
            current->free ? "FREE" : "USED"
        );

        if (length > 0) {
            size_t amount = (size_t)length;

            if (amount >= sizeof(buffer)) {
                amount = sizeof(buffer) - 1;
            }

            write_all(STDOUT_FILENO, buffer, amount);
        }

        current = current->next;
        index++;
    }
}

/* Check heap structure and block consistency */
int heap_check(void)
{
    if ((head == NULL) != (tail == NULL)) {
        return 0;
    }

    if (head == NULL) {
        return total_heap_bytes == 0;
    }

    block_t *current = head;
    block_t *last = NULL;

    while (current != NULL) {
        uintptr_t address = (uintptr_t)current;

        if (address % ALIGNMENT != 0) {
            return 0;
        }

        if (current->magic != MAGIC) {
            return 0;
        }

        if (current->free != 0 && current->free != 1) {
            return 0;
        }

        if (current->size % ALIGNMENT != 0) {
            return 0;
        }

        if (address > UINTPTR_MAX - HDR ||
            address + HDR > UINTPTR_MAX - current->size) {
            return 0;
        }

        uintptr_t end = address + HDR + current->size;

        if (current->next != NULL) {
            uintptr_t next_address = (uintptr_t)current->next;

            if (next_address <= address || next_address < end) {
                return 0;
            }
        }

        last = current;
        current = current->next;
    }

    return last == tail && tail->next == NULL;
}
