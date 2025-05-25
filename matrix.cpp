#include <iostream>   
#include <time.h>
#include <string.h>
#include <math.h>
using namespace std;

#define MATRIX_SIZE 1024

int main() {
    clock_t start, finish;
    clock_t start_new, finish_new;

    register int i, j, k;

    int (*a)[MATRIX_SIZE], (*b)[MATRIX_SIZE];
    a = new int[MATRIX_SIZE][MATRIX_SIZE];
    b = new int[MATRIX_SIZE][MATRIX_SIZE];


    for (i = 0; i < MATRIX_SIZE; i++) {
        for (j = 0; j < MATRIX_SIZE; j++) {
            a[i][j] = i % (j + 1);
            b[i][j] = i / (j + 1);
        }
    }

    int (*c)[MATRIX_SIZE], (*d)[MATRIX_SIZE];
    c = new int[MATRIX_SIZE][MATRIX_SIZE];
    d = new int[MATRIX_SIZE][MATRIX_SIZE];

    // initialize
    memset(d, 0, MATRIX_SIZE * MATRIX_SIZE * sizeof(int));

    start_new = clock();

    // TODO: Your optimized code:
    //======================================================
    constexpr size_t BLOCK_DIM = 128;

    // 计算总块数
    constexpr size_t NUM_BLOCKS = MATRIX_SIZE / BLOCK_DIM;

    // 主计算循环
    for (register size_t block_idx = 0; block_idx < NUM_BLOCKS; ++block_idx) {
        register size_t start_col = block_idx * BLOCK_DIM;
        register size_t end_col = start_col + BLOCK_DIM;

        // 按行处理
        for (register size_t row = 0; row < MATRIX_SIZE; row++) {
            // 缓存行首地址
            const int* a_row = a[row];
            register int* d_row = d[row];

            // 块内循环展开（每次处理4个元素）
            register size_t col = start_col;
            for (; col + 3 < end_col; col += 4) {
                register int temp0 = a_row[col];
                register int temp1 = a_row[col + 1];
                register int temp2 = a_row[col + 2];
                register int temp3 = a_row[col + 3];

                // 内部矩阵乘法
                for (register size_t k = 0; k < MATRIX_SIZE; k++) {
                    d_row[k] += temp0 * b[col][k] +
                        temp1 * b[col + 1][k] +
                        temp2 * b[col + 2][k] +
                        temp3 * b[col + 3][k];
                }
            }

            // 处理剩余元素
            for (; col < end_col; col++) {
                register int temp = a_row[col];
                for (register size_t k = 0; k < MATRIX_SIZE; k++) {
                    d_row[k] += temp * b[col][k];
                }
            }
        }
    }
    // Stop here.
    //======================================================

    finish_new = clock();

    int (*a_)[MATRIX_SIZE], (*b_)[MATRIX_SIZE];
    a_ = new int[MATRIX_SIZE][MATRIX_SIZE];
    b_ = new int[MATRIX_SIZE][MATRIX_SIZE];


    for (i = 0; i < MATRIX_SIZE; i++) {
        for (j = 0; j < MATRIX_SIZE; j++) {
            a_[i][j] = i % (j + 1);
            b_[i][j] = i / (j + 1);
        }
    }

    // initialize
    memset(c, 0, MATRIX_SIZE * MATRIX_SIZE * sizeof(int));

    start = clock();
    for (i = 0; i < MATRIX_SIZE; i++)
        for (j = 0; j < MATRIX_SIZE; j++)
            for (k = 0; k < MATRIX_SIZE; k++)
                c[i][j] += a_[i][k] * b_[k][j];
    finish = clock();


    // compare
    for (i = 0; i < MATRIX_SIZE; i++) {
        for (j = 0; j < MATRIX_SIZE; j++) {
            if (c[i][j] != d[i][j]) {
                cout << "you have got an error in algorithm modification!" << endl;
                exit(1);
            }
        }
    }

    cout << "time spent for original method : " << (double)(finish - start) / CLOCKS_PER_SEC << " s" << endl;
    cout << "time spent for new method : " << (double)(finish_new - start_new) / CLOCKS_PER_SEC << " s" << endl;
    cout << "time ratio of performance optimization : " << (double)(finish - start) / (finish_new - start_new) << endl;
    return 0;
}