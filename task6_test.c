
#include <stdio.h>
#include <string.h>
#include "myalloc.h"

int main(void)
{
    /* Test calloc */
    int *numbers = my_calloc(5, sizeof(int));

    if (numbers == NULL) {
        printf("FAIL: calloc allocation failed\n");
        return 1;
    }

    int zeroed = 1;

    for (int i = 0; i < 5; i++) {
        if (numbers[i] != 0) {
            zeroed = 0;
        }
    }

    if (zeroed) {
        printf("PASS: calloc initialized memory to zero\n");
    } else {
        printf("FAIL: calloc did not initialize memory\n");
        return 1;
    }

    /* Test realloc */
    char *text = my_malloc(20);

    if (text == NULL) {
        printf("FAIL: Initial allocation failed\n");
        return 1;
    }

    strcpy(text, "Hello Bashan");

    char *resized = my_realloc(text, 100);

    if (resized == NULL) {
        printf("FAIL: realloc failed\n");
        return 1;
    }

    if (strcmp(resized, "Hello Bashan") == 0) {
        printf("PASS: realloc preserved existing data\n");
    } else {
        printf("FAIL: realloc lost existing data\n");
        return 1;
    }

    my_free(numbers);
    my_free(resized);

    printf("PASS: Memory released successfully\n");

    return 0;
}
