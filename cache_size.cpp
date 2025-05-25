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
const size_t STRIDE = 32;  // 步长调整为 16 个 int（64 字节，匹配典型缓存行大小）
const size_t ITERATIONS = 1000000000;  // 保持一致的访问次数

int main() {
    cout<< "STRDIE: " << STRIDE << endl;
    // 绑定到 CPU 核心 0
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask);
    if(sched_setaffinity(0, sizeof(mask), &mask) < 0) {
        cout << "Error: sched_setaffinity failed" << endl;
        return -1;
    }

    // 测试的数组大小（以元素个数为单位），从 1KB 到 64MB
    std::vector<size_t> sizes = { (1 << 10) , (2 << 10) , (4 << 10) ,
                                  (8 << 10) , (16 << 10) ,  // 1KB - 16KB
                                  (32 << 10) , (64 << 10) , (128 << 10) ,
                                  (256 << 10) ,                          // 32KB - 256KB
                                  (512 << 10) , (1 << 20) , (2 << 20) ,
                                  (4 << 20) ,                            // 512KB - 4MB
                                  (8 << 20) , (16 << 20) , (32 << 20) ,
                                  (64 << 20)  };                         // 8MB - 64MB

    //std::cout << "Measuring cache sizes with optimizations (no huge pages)...\n";
    //std::cout << "Size (KB)\tSize (MB)\tTotal time(s)\t\n";

    // 创建一个大小为 size 的数组
    size_t max_size = 64 << 20;
    auto array_test = new uint8_t[max_size];
    memset(array_test, 0, max_size * sizeof(uint8_t));

    for (const auto size : sizes) {

        size_t size_mask = size - 1;
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);
        for (register size_t i = 0; i < ITERATIONS * STRIDE; i += STRIDE) {
            array_test[i & size_mask] = i;
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        //printf("start: %ld %ld\n", start.tv_sec, start.tv_nsec);
        //printf("end: %ld %ld\n", end.tv_sec, end.tv_nsec);
        double duration_time = (end.tv_sec - start.tv_sec) * 1e3 + (end.tv_nsec - start.tv_nsec) / 1e6;
        
        std::cout << size * sizeof(uint8_t) / 1024 << "\t\t" << size * sizeof(uint8_t) / 1024 / 1024 << "\t\t"
            << duration_time << "\t\t" <<std::endl;

    }
    delete[] array_test;

    //std::cout << "\nAnalysis:\n";
    //std::cout << "Look for significant increases in access time to identify cache boundaries.\n";
    //std::cout << "Typical L1 DCache: 32KB - 64KB, L2 Cache: 256KB - 1MB, L3 Cache: 4MB - 32MB (varies by CPU).\n";

    return 0;
}