# Memory Allocator – SCT212-0450/2024

**Student Name:** John Karusa  
**Registration Number:** SCT212-0450/2024  
**Course:** ICS 2305 – Systems Programming  
**Project:** Custom Memory Allocator

## 1. Project Description

This project implements a custom memory allocator in C using `sbrk()` to request memory from the operating system. It provides memory allocation, deallocation, zero-initialized allocation, resizing, heap statistics, and heap consistency checking.

The allocator supports both First-Fit and Best-Fit allocation strategies. It also implements block splitting and coalescing to improve memory usage and reduce fragmentation.

## 2. Project Structure

The project contains the following files:

| File | Description |
|---|---|
| `README.md` | Project documentation |
| `REPORT.md` | Project report and benchmark analysis |
| `Makefile` | Builds and runs the tests |
| `myalloc.h` | Allocator function declarations |
| `myalloc.c` | Memory allocator implementation |
| `test_myalloc.c` | Provided test suite |
| `bench.c` | Benchmark comparing allocation strategies |
| `.gitignore` | Excludes generated files from Git |

## 3. Building and Running

To compile and run the official tests:

```bash
make clean
make
```

To compile and run the benchmark:

```bash
gcc -std=c11 -Wall -Wextra -O2 -o bench myalloc.c bench.c
./bench
```

To remove generated files:

```bash
make clean
```

## 4. Task Completion

| Task | Description | Status |
|---|---|---|
| Task 1 | Basic memory allocation | Complete |
| Task 2 | Freeing memory and block management | Complete |
| Task 3 | Block splitting and memory reuse | Complete |
| Task 4 | Coalescing adjacent free blocks | Complete |
| Task 5 | `calloc()` and `realloc()` implementation | Complete |
| Task 6 | Heap growth using `sbrk()` | Complete |
| Task 7 | First-Fit and Best-Fit allocation policies | Complete |
| Task 8 | Randomized stress testing and benchmarking | Complete |

## 5. Final Test Results

The project was tested using:

```bash
make clean
make
```

The final test output was:

```text
T8 randomized stress test
  PASS  no data corruption during 20000 operations
  PASS  everything freed
  PASS  fully coalesced (few free blocks remain)
  PASS  heap consistent

Result: 29 passed, 0 failed
```

All 29 official tests passed without failures.

## 6. Design Explanation

The allocator uses a block-based memory management approach. Each block contains a header followed by a payload area. The header stores information such as the block size, whether the block is free, and a pointer to the next block. Both the header size and payload sizes are aligned to 16 bytes to ensure that allocated memory addresses satisfy alignment requirements.

The blocks are organized using a singly linked list, with each block pointing to the next block in memory-management order. When a memory request is received, the allocator searches the list for a suitable free block. First-Fit selects the first block large enough for the request, while Best-Fit selects the smallest suitable free block.

If a free block is larger than the requested size, it can be split into an allocated block and a smaller free block, provided the remaining space is large enough to hold another block. When memory is released, the allocator marks the block as free and checks whether physically adjacent free blocks can be merged.

Coalescing reduces external fragmentation by combining neighbouring free blocks into larger blocks. However, the allocator checks actual memory addresses before merging because consecutive blocks in the linked list are not necessarily physically adjacent. When more memory is needed, `sbrk()` is used to extend the heap.

## 7. Questions

**1. Why must the header size be a multiple of 16 as well as the payload sizes?**

The header size must be a multiple of 16 to ensure that the payload starts at a 16-byte aligned memory address. This keeps allocated memory properly aligned and prevents alignment-related errors. Both the header and payload sizes must therefore maintain 16-byte alignment.

**2. Why can `printf()` inside `my_malloc()` cause infinite recursion?**

`printf()` may internally request memory using the standard library's `malloc()`. If the allocator or its output routines indirectly trigger another allocation, this can cause repeated calls back into the allocator, resulting in infinite recursion. Using `write()` for output avoids this problem because it does not require dynamic memory allocation.

**3. Your `my_free()` is O(n). Which extra field would let you find a block's previous neighbor in O(1), and what is the name of the technique?**

Adding a `prev` pointer to each block header would allow the allocator to access the previous block directly in O(1) time. This technique is called a doubly linked list. It makes backward traversal and finding the previous list node faster.

**4. Explain the difference between internal and external fragmentation, and name one place in your allocator where each one comes from.**

Internal fragmentation is wasted space inside an allocated block, for example, when a requested size is rounded up to a 16-byte boundary. External fragmentation occurs when free memory is divided into smaller non-contiguous blocks, making it difficult to satisfy large allocation requests. It can occur when blocks are freed in different locations and cannot be merged.

**5. Why does `sbrk()` memory never get returned to the OS in your allocator? Describe how you could give back memory at the end of the heap.**

The allocator does not return memory to the operating system because it does not implement heap shrinking with `brk()`. It keeps freed blocks available for future allocations instead. To return memory, the allocator could detect when the final block at the end of the heap is free and move the program break backwards using `brk()`.

**6. Two blocks are consecutive in your list but not adjacent in memory. How did this happen, and what does your coalescing code do about it?**

This can happen when separate `sbrk()` calls create memory regions with gaps between them, or when blocks are added to the list in separate heap-growth operations. The coalescing code checks the actual memory addresses of neighbouring blocks before merging them. It merges only physically adjacent free blocks and leaves blocks separated by gaps unchanged.

## 8. Known Limitations

- The allocator uses `sbrk()` and `brk()`, which are legacy interfaces.
- Freed memory is reused internally but is not returned to the operating system.
- The linked-list search makes allocation and deallocation potentially O(n).
- The allocator is not designed for concurrent multithreaded access.
- Fragmentation may still occur depending on the allocation pattern.

## 9. Sources and AI Declaration

**Sources:**
- Systems Programming lecture notes.
- Linux manual pages for `sbrk()` and `brk()`.
- C programming documentation.

**AI Declaration:**

AI tools were used for guidance, explanations, debugging assistance, and documentation preparation. The implementation was tested using the provided test suite and additional custom tests.