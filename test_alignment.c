
#include <stdio.h>
#include <stdint.h>
#include "myalloc.h"

int main(void)
{
    void *ptrs[256];
    int passed = 0;
    int failed = 0;

    for (size_t i = 0; i < 256; i++) {
        ptrs[i] = my_malloc(i + 1);

        if (ptrs[i] == NULL) {
            failed++;
            continue;
        }

        if ((uintptr_t)ptrs[i] % 16 == 0) {
            passed++;
        } else {
            failed++;
        }
    }

    for (size_t i = 0; i < 256; i++) {
        if (ptrs[i] != NULL) {
            my_free(ptrs[i]);
        }
    }

    printf("Alignment tests passed: %d\n", passed);
    printf("Alignment tests failed: %d\n", failed);

    if (!heap_check()) {
        printf("Heap consistency: FAIL\n");
        return 1;
    }

    printf("Heap consistency: PASS\n");

    return failed == 0 ? 0 : 1;
}
