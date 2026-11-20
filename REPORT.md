# MEMORY ALLOCATOR PROJECT REPORT

**Student Name:** John Karusa  
**Registration Number:** SCT212-0450/2024  
**Course:** ICS 2305 – Systems Programming  
**Project:** Custom Memory Allocator

## 1. Introduction

This project involved designing and implementing a custom memory allocator in the C programming language. The allocator uses `sbrk()` to request memory from the operating system and provides functions for allocating, freeing, resizing and managing memory blocks.

The project also compares First-Fit and Best-Fit allocation strategies and evaluates their performance against the standard C library allocator.

## 2. Objectives

- Implement a custom memory allocator using `sbrk()`.
- Implement memory allocation and deallocation.
- Support `calloc()` and `realloc()`.
- Implement block splitting and coalescing.
- Compare First-Fit and Best-Fit allocation policies.
- Test memory consistency and measure fragmentation.
- Benchmark the custom allocator against the standard library allocator.

## 3. Implementation and Testing

The allocator was implemented using a linked list of memory block headers. Each block contains information about its size, allocation status and next block.

The implementation includes:

- First-Fit and Best-Fit allocation policies.
- Memory block splitting and coalescing.
- 16-byte memory alignment.
- Heap statistics and consistency checking.
- `my_malloc()`, `my_free()`, `my_calloc()` and `my_realloc()`.

The official test suite was executed using:

```bash
make clean
make
```

**Test results:**

- Total tests passed: 29
- Total tests failed: 0
- Randomized stress test: 20,000 operations completed successfully.

## 4. Benchmark Results

The benchmark was conducted using 50,000 allocation and deallocation operations. The three approaches tested were First-Fit, Best-Fit and the standard C library allocator.

### 4.1 Performance comparison

| Allocation method | Execution time |
|---|---:|
| First-Fit | 0.207507 seconds |
| Best-Fit | 0.241442 seconds |
| Standard library malloc | 0.003066 seconds |

### 4.2 Memory usage and fragmentation

| Measurement | First-Fit | Best-Fit |
|---|---:|---:|
| Heap bytes | 851,968 | 851,968 |
| Used bytes | 694,192 | 695,264 |
| Free bytes | 128,048 | 130,400 |
| Total blocks | 929 | 822 |
| Free blocks | 258 | 151 |
| Largest free block | 14,880 | 56,720 |
| Fragmentation | 88.38% | 56.50% |
| Heap check | PASS | PASS |

Fragmentation was calculated using:

**Fragmentation = 1 − (Largest free block / Total free bytes)**

### 4.3 Benchmark analysis

The results show that First-Fit was faster than Best-Fit, taking 0.207507 seconds compared to 0.241442 seconds. This is because First-Fit stops searching when it finds the first suitable free block, while Best-Fit searches for the smallest suitable block.

Best-Fit produced lower external fragmentation at 56.50%, compared to 88.38% for First-Fit. It also resulted in fewer free blocks and a larger largest-free-block size. The standard library allocator was considerably faster, completing the benchmark in 0.003066 seconds. This demonstrates that the custom allocator works correctly but has additional search and management overhead.

## 5. Results and Discussion

All 29 official tests passed successfully. The tests confirmed that memory allocation, deallocation, alignment, block splitting, coalescing, heap growth, `calloc()`, `realloc()` and both allocation policies worked as expected.

The randomized stress test also completed 20,000 operations without data corruption. Both First-Fit and Best-Fit passed the heap consistency checks during benchmarking.

## 6. Conclusion

The project successfully demonstrated how memory allocation can be implemented in C using low-level memory management techniques. The allocator supports two allocation strategies, block splitting, coalescing and heap statistics.

The benchmark showed that First-Fit offers better execution time between the two custom strategies, while Best-Fit reduces fragmentation. Although the standard library allocator performed much faster, the project provided practical experience in memory management, linked lists, heap organization and systems programming.

## 7. References

1. ICS 2305 Systems Programming lecture notes.
2. Linux manual pages: `sbrk(2)` and `brk(2)`.
3. C programming language documentation.
4. Provided Memory Allocator assignment instructions.

## 8. AI Declaration

AI tools were used for guidance, explanations, debugging assistance and documentation preparation. The implementation was compiled and evaluated using the provided test suite and additional custom tests.