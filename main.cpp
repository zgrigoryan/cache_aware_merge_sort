#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <algorithm> 

// Cache parameteres 
constexpr int CACHE_LINE_SIZE = 128;   // in bytes
constexpr int L1_CACHE_SIZE = 65536;     // L1 data cache (64 KB)
constexpr int BLOCK_SIZE = L1_CACHE_SIZE / sizeof(int);  // number of ints that fit in L1 cache

// ---------------------------------------------------
// 1. Cache-Aware Merge Sort (with blocking)
// ---------------------------------------------------

// A merge function that processes blocks of elements aligned to cache lines.
void blockedMerge(std::vector<int>& arr, std::vector<int>& temp, int left, int mid, int right) {
    int i = left, j = mid, k = left;

    while (i < mid && j < right) {
        int block_end_i = std::min(i + BLOCK_SIZE, mid);
        int block_end_j = std::min(j + BLOCK_SIZE, right);

        while (i < block_end_i && j < block_end_j) {
            if (arr[i] < arr[j])
                temp[k++] = arr[i++];
            else
                temp[k++] = arr[j++];
        }
    }

    while (i < mid) temp[k++] = arr[i++];
    while (j < right) temp[k++] = arr[j++];

    std::memcpy(&arr[left], &temp[left], (right - left) * sizeof(int));
}

void cacheAwareMergeSort(std::vector<int>& arr, std::vector<int>& temp, int left, int right) {
    if (right - left <= 1)
        return;

    int mid = left + (right - left) / 2;

    cacheAwareMergeSort(arr, temp, left, mid);
    cacheAwareMergeSort(arr, temp, mid, right);

    // merge the two sorted halves using blocked merge 
    blockedMerge(arr, temp, left, mid, right);
}

void runCacheAwareMergeSort(std::vector<int>& arr) {
    std::vector<int> temp(arr.size());
    cacheAwareMergeSort(arr, temp, 0, arr.size());
}

// ---------------------------------------------------
// 2. Standard Merge Sort 
// ---------------------------------------------------
void standardMerge(std::vector<int>& arr, std::vector<int>& temp, int left, int mid, int right) {
    int i = left, j = mid, k = left;
    while (i < mid && j < right) {
        if (arr[i] < arr[j])
            temp[k++] = arr[i++];
        else
            temp[k++] = arr[j++];
    }
    while (i < mid) temp[k++] = arr[i++];
    while (j < right) temp[k++] = arr[j++];
    std::memcpy(&arr[left], &temp[left], (right - left) * sizeof(int));
}

void standardMergeSort(std::vector<int>& arr, std::vector<int>& temp, int left, int right) {
    if (right - left <= 1)
        return;
    int mid = left + (right - left) / 2;
    standardMergeSort(arr, temp, left, mid);
    standardMergeSort(arr, temp, mid, right);
    standardMerge(arr, temp, left, mid, right);
}

void runStandardMergeSort(std::vector<int>& arr) {
    std::vector<int> temp(arr.size());
    standardMergeSort(arr, temp, 0, arr.size());
}

// ---------------------------------------------------
// 3. Cache-Oblivious Merge Sort (Recursive, contiguous)
// ---------------------------------------------------
//
// This implementation uses a standard recursive merge sort that operates
// entirely on contiguous segments. The merging step uses the C++ STL function
// std::inplace_merge, which works on contiguous data without explicit tuning.
void cacheObliviousMergeSort(std::vector<int>& arr, int left, int right) {
    if (right - left <= 1)
        return;
    
    int mid = left + (right - left) / 2;
    
    cacheObliviousMergeSort(arr, left, mid);
    cacheObliviousMergeSort(arr, mid, right);
    
    // std::inplace_merge works on contiguous ranges and is designed to be efficient.
    std::inplace_merge(arr.begin() + left, arr.begin() + mid, arr.begin() + right);
}

void runCacheObliviousMergeSort(std::vector<int>& arr) {
    cacheObliviousMergeSort(arr, 0, arr.size());
}

// ---------------------------------------------------
// Benchmark function
// ---------------------------------------------------
void benchmarkSort(void (*sortFunction)(std::vector<int>&), std::vector<int>& arr, const std::string& name) {
    auto start = std::chrono::high_resolution_clock::now();
    sortFunction(arr);
    auto end = std::chrono::high_resolution_clock::now();
    
    double duration = std::chrono::duration<double, std::milli>(end - start).count();
    std::cout << name << " took " << duration << " ms\n";
}

int main() {
    const size_t N = 1'000'000; 
    std::vector<int> arr_standard(N), arr_cacheAware(N), arr_cacheOblivious(N);

    // Initialize the arrays with identical random data.
    for (size_t i = 0; i < N; i++) {
        int value = rand() % N;
        arr_standard[i] = value;
        arr_cacheAware[i] = value;
        arr_cacheOblivious[i] = value;
    }

    std::cout << "Benchmarking...\n";
    benchmarkSort(runStandardMergeSort, arr_standard, "Standard Merge Sort");
    benchmarkSort(runCacheAwareMergeSort, arr_cacheAware, "Cache-Aware Merge Sort");
    benchmarkSort(runCacheObliviousMergeSort, arr_cacheOblivious, "Cache-Oblivious Merge Sort");

    return 0;
}
