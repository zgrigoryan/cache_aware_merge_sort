#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <algorithm>
#include <random>
#include <thread>
#include <atomic>
#include "kaizen.h" // Zen library for command line arguments

// Cache parameters
constexpr int CACHE_LINE_SIZE = 128;   // in bytes
constexpr int L1_CACHE_SIZE = 65536;     // L1 data cache (64 KB)
constexpr int BLOCK_SIZE = L1_CACHE_SIZE / sizeof(int);  // number of ints that fit in L1 cache

// Random number generator to initialize arrays
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(0, 1000000);

// ---------------------------------------------------
// 1. Cache-Aware Merge Sort (with blocking)
// ---------------------------------------------------
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
void cacheObliviousMergeSort(std::vector<int>& arr, int left, int right) {
    if (right - left <= 1)
        return;
    
    int mid = left + (right - left) / 2;
    
    cacheObliviousMergeSort(arr, left, mid);
    cacheObliviousMergeSort(arr, mid, right);
    
    std::inplace_merge(arr.begin() + left, arr.begin() + mid, arr.begin() + right);
}

void runCacheObliviousMergeSort(std::vector<int>& arr) {
    cacheObliviousMergeSort(arr, 0, arr.size());
}


void benchmarkSort(void (*sortFunction)(std::vector<int>&), std::vector<int>& arr, const std::string& name, int iterations) {
    std::vector<int> temp(arr.size());

    double totalDuration = 0.0;  

    for (int i = 0; i < iterations; i++) {
        std::generate(arr.begin(), arr.end(), [&] { return dis(gen); });

        auto start = std::chrono::high_resolution_clock::now();
        
        sortFunction(arr);  

        auto end = std::chrono::high_resolution_clock::now();

        double duration = std::chrono::duration<double, std::milli>(end - start).count();
        totalDuration += duration; 
    }

    double averageDuration = totalDuration / iterations;
    std::cout << name << " took an average of " << averageDuration << " ms over " << iterations << " iterations.\n";
}


int main(int argc, char* argv[]) {
    zen::cmd_args args(argv, argc);

    if (!args.is_present("--size") || !args.is_present("--iterations")) {
        std::cerr << "Usage: " << argv[0]
                  << " --size <array_size> --iterations <num_iterations>"
                  << std::endl;
        return 1;
    }

    int size = std::stoi(args.get_options("--size")[0]);
    int iterations = std::stoi(args.get_options("--iterations")[0]);
    if (size <= 0 || iterations <= 0) {
        std::cerr << "Array size and iterations must be positive integers."
                  << std::endl;
        return 1;
    }

    std::vector<int> arr_standard(size), arr_cacheAware(size), arr_cacheOblivious(size);

    for (size_t i = 0; i < size; i++) {
        int value = dis(gen);
        arr_standard[i] = value;
        arr_cacheAware[i] = value;
        arr_cacheOblivious[i] = value;
    }

    std::cout << "Benchmarking...\n";

    benchmarkSort(runStandardMergeSort, arr_standard, "Standard Merge Sort", iterations);
    benchmarkSort(runCacheAwareMergeSort, arr_cacheAware, "Cache-Aware Merge Sort", iterations);
    benchmarkSort(runCacheObliviousMergeSort, arr_cacheOblivious, "Cache-Oblivious Merge Sort", iterations);

    return 0;
}
