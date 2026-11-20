
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "myalloc.h"

int main(void)
{
    char *ptr = my_malloc(100);

    if (ptr == NULL) {
        printf("FAIL: Allocation returned NULL\n");
        return 1;
    }

    if ((uintptr_t)ptr % 16 != 0) {
        printf("FAIL: Pointer is not 16-byte aligned\n");
        return 1;
    }

    memset(ptr, 'A', 100);

    printf("PASS: Memory allocated successfully\n");
    printf("PASS: Pointer is 16-byte aligned\n");
    printf("PASS: Wrote 100 bytes successfully\n");

    return 0;
}
