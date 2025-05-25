#include <iostream>
#include <emmintrin.h> // _mm_clflush
#include <vector>
#include <algorithm>
#include <deque>
#include <random>

// 获取时间戳计数器 (RDTSC)
static inline uint64_t get_cycles() {
    unsigned int lo, hi;
    asm volatile("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

enum Strategy { LRU, FIFO, OPT };

// 测试缓存替换策略
void test_replacement_strategy(Strategy strategy, int cache_size = 32 * 1024, int line_size = 64, int way = 8) {
    const int SETS = cache_size / line_size / way; // 组数
    const int ARRAY_SIZE = cache_size / sizeof(int); // 数组元素数
    const int ITERATIONS = 10000;

    volatile int* array = new volatile int[ARRAY_SIZE](); // 动态分配并初始化为0
    std::vector<uint64_t> hit_times(ITERATIONS), miss_times(ITERATIONS); // 存储每次测量结果

    // 清空缓存，确保Miss测试从冷缓存开始
    for (int i = 0; i < ARRAY_SIZE; i += line_size / sizeof(int)) {
        _mm_clflush((void*)&array[i]);
    }
    _mm_mfence();

    // 生成访问序列
    int stride = SETS; // 确保映射到同一组
    std::cout << "Testing " << (strategy == LRU ? "LRU" : strategy == FIFO ? "FIFO" : "OPT") << " behavior (Miss-like test)...\n";

    // 模拟缓存组
    std::deque<int> cache(way, -1); // 使用deque模拟缓存
    std::vector<int> access_sequence; // 访问序列
    access_sequence.reserve(ITERATIONS * (way + 1));

    // 先测试Miss-like行为
    for (int i = 0; i < ITERATIONS; i++) {
        // 填满组并多访问一次以触发替换
        for (int j = 0; j <= way; j++) {
            int addr = j * stride;
            access_sequence.push_back(addr);
            array[addr] = j; // 写入数据
        }
        _mm_mfence();

        // 测量访问第一个元素的时间，预期可能为Miss
        uint64_t start = get_cycles();
        int dummy = array[0]; // 测试是否被替换
        _mm_mfence();
        uint64_t end = get_cycles();
        miss_times[i] = end - start;

        // 更新缓存状态
        int new_addr = access_sequence.back();
        if (strategy == LRU) {
            auto it = std::find(cache.begin(), cache.end(), new_addr);
            if (it != cache.end()) cache.erase(it);
            else if (cache.size() == way) cache.pop_front();
            cache.push_back(new_addr);
        }
        else if (strategy == FIFO) {
            if (cache.size() < way) cache.push_back(new_addr);
            else {
                cache.pop_front();
                cache.push_back(new_addr);
            }
        }
        else if (strategy == OPT) {
            if (cache.size() < way) cache.push_back(new_addr);
            else {
                // 预测未来最远访问
                int furthest_idx = 0, max_dist = -1;
                for (int k = 0; k < way; k++) {
                    auto it = std::find(access_sequence.begin() + i * (way + 1), access_sequence.end(), cache[k]);
                    int dist = (it == access_sequence.end()) ? ITERATIONS : (it - access_sequence.begin());
                    if (dist > max_dist) {
                        max_dist = dist;
                        furthest_idx = k;
                    }
                }
                cache[furthest_idx] = new_addr;
            }
        }
    }

    // 计算Miss时间中位数
    std::sort(miss_times.begin(), miss_times.end());
    uint64_t miss_time = miss_times[ITERATIONS / 2];

    // 清空缓存，为Hit测试准备
    for (int i = 0; i < ARRAY_SIZE; i += line_size / sizeof(int)) {
        _mm_clflush((void*)&array[i]);
    }
    _mm_mfence();

    // 测试Hit基线
    std::cout << "Testing Cache Hit baseline...\n";
    for (int i = 0; i < ITERATIONS; i++) {
        int dummy = array[0]; // 预热，确保在Cache中
        _mm_mfence();
        uint64_t start = get_cycles();
        dummy = array[0]; // Hit
        _mm_mfence();
        uint64_t end = get_cycles();
        hit_times[i] = end - start;
    }
    std::sort(hit_times.begin(), hit_times.end());
    uint64_t hit_time = hit_times[ITERATIONS / 2];

    // 输出结果
    std::cout << "Median Hit latency: " << hit_time << " cycles\n";
    std::cout << "Median Miss-like latency (" << (strategy == LRU ? "LRU" : strategy == FIFO ? "FIFO" : "OPT") << "): " << miss_time << " cycles\n";

    // 分析
    if (miss_time > hit_time * 2) {
        std::cout << "Likely " << (strategy == LRU ? "LRU" : strategy == FIFO ? "FIFO" : "OPT") << ": First element replaced.\n";
    }
    else {
        std::cout << "Possibly Random or mismatch: No clear pattern.\n";
    }

    delete[] array;
}

int main() {
    std::cout << "Testing L1 DCache Replacement Strategy (32KB, 8-way):\n";
    test_replacement_strategy(LRU);
    test_replacement_strategy(FIFO);
    test_replacement_strategy(OPT);
    return 0;
}