# 🚀 Merge Sort Benchmark: Cache-Aware vs. Cache-Oblivious

This project benchmarks **three variations of Merge Sort**:
- Standard Merge Sort
- Cache-Aware Merge Sort (blocked merge)
- Cache-Oblivious Merge Sort

Its primary goal is to demonstrate the **performance implications of cache utilization** in sorting algorithms on modern hardware. The benchmark helps visualize how data locality and memory access patterns impact runtime, especially with respect to **L1 cache** behavior.

---

## 🧠 Background

### 🧩 Standard Merge Sort
A classical divide-and-conquer algorithm that recursively splits an array into halves and merges them in sorted order. It does not take hardware cache behavior into account.

### 📦 Cache-Aware Merge Sort
This version manually **blocks memory accesses** to align with the CPU's L1 data cache. The merge step operates on **cache-sized blocks** to reduce cache misses and improve performance.

- It assumes knowledge of the cache line size and L1 cache size.
- The array is merged in chunks that fit into the L1 cache (e.g., 64 KB), improving temporal and spatial locality.

### 🌀 Cache-Oblivious Merge Sort
This version doesn't explicitly optimize for cache but still takes advantage of caching **implicitly through recursive division**. It relies on the memory hierarchy's natural behavior, breaking the array into smaller subproblems that eventually fit into the cache.

---

## 📂 Project Structure

```
.
├── main.cpp           # The core benchmark and sorting implementations
├── CMakeLists.txt     # Build configuration
├── README.md          # You're reading it!
└── kaizen.h           # Zen library for command-line parsing (must be added manually or via submodule)
```

---

## 🔧 Build Instructions

### Prerequisites

- C++17 compatible compiler (e.g., `g++`, `clang++`, or MSVC)
- [CMake](https://cmake.org) ≥ 3.10
- The [`kaizen.h`](https://github.com/kaizen) library (a lightweight command-line argument parser)

### 1. Clone the repository

```bash
git clone https://github.com/zgrigoryan/cache_aware_oblivious_merge_sort.git
cd cache_aware_oblivious_merge_sort
```

> **Note**: This project uses [`kaizen.h`](https://github.com/kaizen), which is not a system-wide library. If it's not included yet, download and place it in the same directory as `main.cpp`.

### 2. Build the project

```bash
mkdir build
cd build
cmake ..
make
```

The executable `Merge_sort_benchmark` will be generated in the `build/` directory.

---

## ▶️ Run Instructions

You can run the benchmark with the following command:

```bash
./Merge_sort_benchmark --size 100000 --iterations 10
```

### Arguments:

| Argument        | Description                                |
|-----------------|--------------------------------------------|
| `--size`        | Number of integers to sort                 |
| `--iterations`  | Number of times each sort is benchmarked   |

### Example:

```bash
./Merge_sort_benchmark --size 500000 --iterations 5
```

---

## 📊 Output

The program will display the **average runtime** (in milliseconds) for each algorithm over the specified number of iterations:

```
Benchmarking...
Standard Merge Sort took an average of 35.21 ms over 10 iterations.
Cache-Aware Merge Sort took an average of 28.14 ms over 10 iterations.
Cache-Oblivious Merge Sort took an average of 29.47 ms over 10 iterations.
```

---

## 📚 About the Zen (Kaizen) Library

This project uses the [Zen (Kaizen)](https://github.com/kaizen) command-line parser (`kaizen.h`) to handle user input via `--size` and `--iterations` flags. It's a single-header C++ library that provides:

- Simple API for parsing command-line options
- Easy integration (just include `kaizen.h`)
- No dependencies


---

## 📄 License

MIT License — Feel free to use, modify, and share this project for educational or performance benchmarking purposes.
