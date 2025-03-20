#include <algorithm>
#include <iostream>
#include <vector>
#include <chrono>
#include <random>
using namespace std; 

// cache-aware merge sort 
// reads input arrays in blocks that fit into the L1 cache (64KB data cache)
// merges the blocks in a cache-aware way

void cache_aware_merge(int* arr, int n) {
    // block size that fits into the L1 cache
    const int block_size = 64 * 1024 / sizeof(int); // 64KB data cache
    // number of blocks
    const int num_blocks = n / block_size + (n % block_size ? 1 : 0);
    // allocate memory for the temporary array
    int* temp = new int[n];
    // sort each block
    for (int i = 0; i < num_blocks; i++) {
        int start = i * block_size;
        int end = min(start + block_size, n);
        sort(arr + start, arr + end);
    }
    // merge the blocks in a cache-aware way
    for (int block_size = 1; block_size < n; block_size *= 2) {
        for (int i = 0; i < n; i += 2 * block_size) {
            int start1 = i;
            int end1 = min(i + block_size, n);
            int start2 = end1;
            int end2 = min(i + 2 * block_size, n);
            int p1 = start1;
            int p2 = start2;
            int p = start1;
            while (p1 < end1 && p2 < end2) {
                if (arr[p1] < arr[p2]) {
                    temp[p++] = arr[p1++];
                } else {
                    temp[p++] = arr[p2++];
                }
            }
            while (p1 < end1) {
                temp[p++] = arr[p1++];
            }
            while (p2 < end2) {
                temp[p++] = arr[p2++];
            }
            for (int j = start1; j < end2; j++) {
                arr[j] = temp[j];
            }
        }
    }

    delete[] temp;
}

// usual merge sort 
// recursively divides the input array into two halves

void merge(int* arr, int n) {
    if (n <= 1) {
        return;
    }
    int mid = n / 2;
    merge(arr, mid);
    merge(arr + mid, n - mid);
    int* temp = new int[n];
    int p1 = 0;
    int p2 = mid;
    int p = 0;
    while (p1 < mid && p2 < n) {
        if (arr[p1] < arr[p2]) {
            temp[p++] = arr[p1++];
        } else {
            temp[p++] = arr[p2++];
        }
    }
    while (p1 < mid) {
        temp[p++] = arr[p1++];
    }
    while (p2 < n) {
        temp[p++] = arr[p2++];
    }
    for (int i = 0; i < n; i++) {
        arr[i] = temp[i];
    }
    delete[] temp;
}

int main(){

    int* arr = new int[20000000];
    for (int i = 0; i < 20000000; i++) {
        arr[i] = rand();
    }

    auto start = chrono::high_resolution_clock::now();
    cache_aware_merge(arr, 20000000);
    auto end = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    cout << "Cache-aware merge sort: " << duration.count() << " ms" << endl;

    for (int i = 0; i < 20000000; i++) {
        arr[i] = rand();
    }

    start = chrono::high_resolution_clock::now();
    merge(arr, 20000000);
    end = chrono::high_resolution_clock::now();

    duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    cout << "Usual merge sort:" << duration.count() << " ms" << endl;

    return 0; 
}