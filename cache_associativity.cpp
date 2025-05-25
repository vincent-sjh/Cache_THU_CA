#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <sched.h>
#include <chrono>
#include <iostream>
#include <vector>
#include <cstdint>
#include <time.h>
#include <cstring>  // For memset

using namespace std;
const size_t ITERATIONS = 1000000000;  // 保持一致的访问次数

int main() {

    // 绑定到 CPU 核心 0
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask);
    if (sched_setaffinity(0, sizeof(mask), &mask) < 0) {
        cout << "Error: sched_setaffinity failed" << endl;
        return -1;
    }


    //std::cout << "Measuring cache sizes with optimizations (no huge pages)...\n";
    //std::cout << "Size (KB)\tSize (MB)\tTotal time(s)\t\n";

    // 创建一个大小为 size 的数组
    size_t max_size = 32 << 11;
    size_t length = max_size / sizeof(uint64_t);
    size_t size_mask = length - 1;
    auto array_test = new uint8_t[max_size];
    memset(array_test, 0, max_size * sizeof(uint8_t));

    for (int n = 2; n <= 12; n++) {

        size_t stride = length / (1 << n) * 2;
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);
        for (register size_t i = 0; i < ITERATIONS * stride; i += stride) {
            array_test[i & size_mask] = i;
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        //printf("start: %ld %ld\n", start.tv_sec, start.tv_nsec);
        //printf("end: %ld %ld\n", end.tv_sec, end.tv_nsec);
        double duration_time = (end.tv_sec - start.tv_sec) * 1e3 + (end.tv_nsec - start.tv_nsec) / 1e6;

        std::cout << n << "\t\t"
            << duration_time << "\t\t" << std::endl;

    }
    delete[] array_test;

    //std::cout << "\nAnalysis:\n";
    //std::cout << "Look for significant increases in access time to identify cache boundaries.\n";
    //std::cout << "Typical L1 DCache: 32KB - 64KB, L2 Cache: 256KB - 1MB, L3 Cache: 4MB - 32MB (varies by CPU).\n";

    return 0;
}