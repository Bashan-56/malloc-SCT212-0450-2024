# Memory Allocator Benchmark Report

**Student:** John Karusa  
**Registration Number:** SCT212-0450/2024  
**Course:** ICS 2305 – Systems Programming

## 1. Introduction

This report evaluates the performance of the custom memory allocator using first-fit and best-fit allocation policies. The results are compared with the standard C library memory allocator.

## 2. Test Environment

- Operating system: Linux Mint
- Compiler: GCC
- Compiler optimization: `-O2`
- Workload: 50,000 randomized allocation and deallocation operations
- Allocation slots: 1,000
- Random seed: 123456789

## 3. Benchmark Results

### First-fit

| Measurement | Result |
|---|---:|
| Execution time | 0.219275 seconds |
| Heap bytes | 851,968 |
| Used bytes | 694,192 |
| Free bytes | 128,048 |
| Total blocks | 929 |
| Free blocks | 258 |
| Largest free block | 14,880 |
| Fragmentation | 88.38% |
| Heap check | PASS |

### Best-fit

| Measurement | Result |
|---|---:|
| Execution time | 0.235621 seconds |
| Heap bytes | 851,968 |
| Used bytes | 695,264 |
| Free bytes | 130,400 |
| Total blocks | 822 |
| Free blocks | 151 |
| Largest free block | 56,720 |
| Fragmentation | 56.50% |
| Heap check | PASS |

### Standard C Library

| Measurement | Result |
|---|---:|
| Execution time | 0.003201 seconds |
| Operations | 50,000 |

The standard library comparison measures execution time only; its internal heap statistics were not collected.

## 4. Fragmentation Calculation

The external fragmentation formula used was:

\[
F=1-\frac{\text{Largest Free Block}}{\text{Total Free Bytes}}
\]

First-fit:

\[
F=1-\frac{14880}{128048}=0.8838
\]

Best-fit:

\[
F=1-\frac{56720}{130400}=0.5650
\]

## 5. Analysis and Discussion

The benchmark shows that first-fit completed the workload in 0.219275 seconds, while best-fit took 0.235621 seconds. First-fit was therefore slightly faster in this experiment because it stops searching as soon as it finds a suitable block. Best-fit searches through the available blocks to find the smallest suitable one, which introduces additional search overhead. However, best-fit produced better fragmentation results. Its fragmentation was 56.50%, compared with 88.38% for first-fit. It also produced fewer total blocks and fewer free blocks. This suggests that best-fit made more effective use of available free space in this workload. The standard C library allocator was considerably faster, completing the operations in 0.003201 seconds. This comparison is limited to the measured workload and machine. Overall, the results demonstrate a trade-off between allocation speed and memory fragmentation.

## 6. Conclusion

Both allocation policies passed the heap consistency checks. First-fit was faster than best-fit, while best-fit achieved lower fragmentation. The standard C library allocator was fastest in this experiment. The benchmark demonstrates the importance of choosing an allocation strategy based on performance and memory utilization requirements.