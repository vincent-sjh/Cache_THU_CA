#include <iostream>
#include <emmintrin.h> // 用于 _mm_clflush
#include <chrono>

int sum_miss = 0;
int sum_hit = 0;

// 获取时间戳计数器 (RDTSC)
static inline uint64_t get_cycles() {
    unsigned int lo, hi;
    asm volatile("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

// 测试L1 DCache写策略
void test_write_strategy() {
    const int SIZE = 1024 * 64; // 64KB，假设L1 Cache为32KB，需调整
    volatile int* array = new volatile int[SIZE](); // 动态分配并初始化为0
    uint64_t hit_time = 0, miss_time = 0;
    const int ITERATIONS = 10000; // 多次测量取平均值

    // 先测试Write Miss
    //std::cout << "Testing Write Miss...\n";
    for (int i = 0; i < ITERATIONS; i++) {
        uint64_t start = get_cycles();
        array[i % SIZE] = i; // 写到不同位置，确保Miss
        _mm_mfence();
        uint64_t end = get_cycles();
        miss_time += (end - start);
    }
    miss_time /= ITERATIONS;

    // 清空Cache
    for (int i = 0; i < SIZE; i += 16) { // 每16字节清空一行
        _mm_clflush((void*)&array[i]);
    }
    _mm_mfence(); // 确保清空完成

    // 再测试Write Hit
    //std::cout << "Testing Write Hit...\n";
    for (int i = 0; i < ITERATIONS; i++) {
        // 确保数据在Cache中，先读
        int dummy = array[0];
        _mm_mfence(); // 内存屏障，确保读完成
        uint64_t start = get_cycles();
        array[0] = i; // 写操作
        _mm_mfence(); // 确保写完成
        uint64_t end = get_cycles();
        hit_time += (end - start);
    }
    hit_time /= ITERATIONS;

    sum_miss += miss_time;
    sum_hit += hit_time;

    // 输出结果
    delete[] array; // 释放内存
}

int main() {
    std::cout << "Using RDTSC:\n";
    for (int i = 0; i < 100; i++) {
        test_write_strategy();
    }
    int hit_time = sum_hit / 100;
    int miss_time = sum_miss / 100;

    // 输出结果
    std::cout << "Average Write Hit latency: " << hit_time << " cycles\n";
    std::cout << "Average Write Miss latency: " << miss_time << " cycles\n";

    // 分析
    if (hit_time < miss_time * 0.7) { // 经验阈值，需根据实际CPU调整
        std::cout << "Likely Write-back: Hit latency much lower than Miss.\n";
    }
    else {
        std::cout << "Likely Write-through: Hit and Miss latencies similar.\n";
    }

    return 0;
}