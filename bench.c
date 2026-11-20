
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "myalloc.h"

#define OPERATIONS 50000
#define SLOTS 1000

static uint32_t random_state = 123456789;

static uint32_t random_number(void)
{
    random_state ^= random_state << 13;
    random_state ^= random_state >> 17;
    random_state ^= random_state << 5;

    return random_state;
}

static double elapsed_time(struct timespec start,
                           struct timespec end)
{
    return (end.tv_sec - start.tv_sec) +
           (end.tv_nsec - start.tv_nsec) / 1000000000.0;
}

static void run_custom_allocator(fit_policy_t policy,
                                 const char *name)
{
    void *slots[SLOTS] = {0};

    random_state = 123456789;
    set_policy(policy);

    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < OPERATIONS; i++) {
        unsigned int index = random_number() % SLOTS;

        if (slots[index] == NULL) {
            size_t size = (random_number() % 2048) + 1;
            slots[index] = my_malloc(size);
        } else if (random_number() % 2 == 0) {
            my_free(slots[index]);
            slots[index] = NULL;
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    heap_stats_t stats;
    heap_stats(&stats);

    double seconds = elapsed_time(start, end);

    double fragmentation = 0.0;

    if (stats.free_bytes > 0) {
        fragmentation =
            1.0 - ((double)stats.largest_free /
                   (double)stats.free_bytes);
    }

    printf("\n%s RESULTS\n", name);
    printf("------------------------------\n");
    printf("Time: %.6f seconds\n", seconds);
    printf("Heap bytes: %zu\n", stats.heap_bytes);
    printf("Used bytes: %zu\n", stats.used_bytes);
    printf("Free bytes: %zu\n", stats.free_bytes);
    printf("Total blocks: %zu\n", stats.blocks);
    printf("Free blocks: %zu\n", stats.free_blocks);
    printf("Largest free block: %zu\n", stats.largest_free);
    printf("Fragmentation: %.4f (%.2f%%)\n",
           fragmentation, fragmentation * 100.0);

    for (int i = 0; i < SLOTS; i++) {
        if (slots[i] != NULL) {
            my_free(slots[i]);
        }
    }

    printf("Heap check: %s\n",
           heap_check() ? "PASS" : "FAIL");
}

static void run_libc_allocator(void)
{
    void *slots[SLOTS] = {0};

    random_state = 123456789;

    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < OPERATIONS; i++) {
        unsigned int index = random_number() % SLOTS;

        if (slots[index] == NULL) {
            size_t size = (random_number() % 2048) + 1;
            slots[index] = malloc(size);
        } else if (random_number() % 2 == 0) {
            free(slots[index]);
            slots[index] = NULL;
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    printf("\nLIBC MALLOC RESULTS\n");
    printf("------------------------------\n");
    printf("Time: %.6f seconds\n",
           elapsed_time(start, end));

    for (int i = 0; i < SLOTS; i++) {
        free(slots[i]);
    }
}

int main(void)
{
    printf("MEMORY ALLOCATOR BENCHMARK\n");
    printf("Operations: %d\n", OPERATIONS);

    run_custom_allocator(FIT_FIRST, "FIRST-FIT");
    run_custom_allocator(FIT_BEST, "BEST-FIT");
    run_libc_allocator();

    return 0;
}
