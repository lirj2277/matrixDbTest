#include <stdio.h>
#include <sys/time.h>

#define B_LOWER_LIMIT 10
#define B_UPPER_LIMIT 50

int g_count = 0;

typedef struct _Row {
    int a;
    int b;
} Row;

typedef struct _RowArr {
    Row row[100];
    int cur;
    int len;
} RowArr;

// 二分查找，查找第一个等于val的值，例如1000，1000，2000，2000，val是2000，那么第一个2000所在位置就是a值跳变点
int binary_search_first_a(const Row *rows, int start_idx, int end_idx, int val) {
    int ret = -1;
    if (rows[end_idx].a < val) {
        printf("not found val:%d\n", val);
        return ret;
    }

    int count = 0;
    while(1) {
        // printf("start:%d, end:%d, count:%d\n", start_idx, end_idx, ++count);
        if (start_idx == end_idx || start_idx + 1 == end_idx) {
             break;
        }

        int cur_idx = (start_idx + end_idx) / 2;

        // 如果cur_idx的a值小于val，那么start_idx向右趋近
        if (rows[cur_idx].a < val) {
            start_idx = cur_idx;
            continue;
        }

        if (rows[cur_idx].a >= val) {
            end_idx = cur_idx;
            continue;
        }
    }

    if (rows[start_idx].a == val) {
        ret = start_idx;
    } else if (rows[end_idx].a == val) {
        ret = end_idx;
    } else {
        ret = -1;
    }
    return ret;
}

// 二分查找，查找最后一个等于val的值，例如1000，1000，2000，2000，val是1000，那么最后一个1000所在位置就是a值跳变点
int binary_search_last_a(const Row *rows, int start_idx, int end_idx, int val) {
    int ret = -1;
    if (rows[end_idx].a < val) {
        printf("not found val:%d\n", val);
        return ret;
    }

    int count = 0;
    while(1) {
        // printf("start:%d, end:%d, count:%d\n", start_idx, end_idx, ++count);
        if (start_idx == end_idx || start_idx + 1 == end_idx) {
            break;
        }

        int cur_idx = (start_idx + end_idx) / 2;

        if (rows[cur_idx].a > val) {
            end_idx = cur_idx;
            continue;
        }

        if (rows[cur_idx].a <= val) {
            start_idx = cur_idx;
            continue;
        }
    }

    if (rows[start_idx].a == val) {
        ret = start_idx;
    } else if (rows[end_idx].a == val) {
        ret = end_idx;
    } else {
        ret = -1;
    }
    return ret;
}

// 二分查找，查找第一个等于val的值，例如3，4，9，10，10，11，val是10，那么第一个10所在位置就是b值跳变点
int binary_search_b(const Row *rows, int start_idx, int end_idx, int val) {
    int ret = -1;
    if (rows[end_idx].b < val) {
        printf("not found val:%d\n", val);
        return ret;
    }

    int count = 0;
    while(1) {
        // printf("start:%d, end:%d, count:%d\n", start_idx, end_idx, ++count);
        if (start_idx == end_idx || start_idx + 1 == end_idx) {
            break;
        }

        int cur_idx = (start_idx + end_idx) / 2;

        if (rows[cur_idx].b < val) {
            start_idx = cur_idx;
            continue;
        }

        if (rows[cur_idx].b >= val) {
            end_idx = cur_idx;
            continue;
        }
    }

    if (rows[start_idx].b == val) {
        ret = start_idx;
    } else if (rows[end_idx].b == val) {
        ret = end_idx;
    } else {
        ret = -1;
    }
    return ret;
}

void task1(const Row *rows, int n_rows) {
    struct timeval start, end;
    gettimeofday(&start, NULL);
    printf("start : %d.%d\n", start.tv_sec, start.tv_usec);
    g_count = 0;

    for (int i = 0; i < n_rows; i++) {
        int a = rows[i].a;
        int b = rows[i].b;

        // todo 这个条件只有与task2性能对比时才可以加，因为其他条件下a不一定是递增的
        if (a > 3000) {
            break;
        }

        g_count++;

        if ((b >= B_LOWER_LIMIT && b < B_UPPER_LIMIT) && (a == 1000 || a == 2000 || a == 3000)) {
            printf("%d,%d\n", a, b);
        }
    }
    gettimeofday(&end, NULL);
    printf("end   : %d.%d\n", end.tv_sec, end.tv_usec);
    printf("cost(us): %d, g_count:%d\n", end.tv_sec*1000000 + end.tv_usec - start.tv_sec*1000000 - start.tv_usec, g_count);
}

/*
 * task2的前提是rows已经按照(a, b)进行过排序，基于这个前提进行性能优化，根据rows数据示例可以发现，rows整体上是先按照a递增，即a为1000的行
 * 一定是在a为2000的行的前面；与此同时，在a相同时，b递增
 *
 * 基于rows已经排序这个前提设计优化方案，方案的核心思想就是尽量减少for循环的次数，方案如下：
 * 1. 既然a是递增的，那么要过滤a为1000、2000、3000的行的话，可以先判断一下a是不是已经大于3000了，如果大于3000就直接break，因为后续不可能
 * 有满足条件的a值了
 * 2. 针对a值的优化，因为a是递增的，那么当a不满足条件时，从当前位置查找，直接跳到下一个满足条件的a值的位置，因为递增，所以我使用了二分查找，
 * 并做了一些调整，因为a值可能会重复，我需要的是第一个要查找的a值的位置
 * 3. 针对b值的优化，a满足条件而b不满足条件，这里有两种场景：一是b超过了上限，例如b >= 50，那么处理和a不满足要求时是一致的，要找到下一个符合
 * 条件的a值的位置，例如当前a是1000，那么就要找第一个2000的位置；二是b低于下限制，例如b < 10,那么需要在当前a值的范围内找第一个b为10的位置，
 * 要达到这个目的，需要做两次二分查找，例如当前a是1000，b < 10，那么需要找到最后一个a为1000的位置，然后在这个范围内查找第一个b为10的位置
 *
 * */
void task2(const Row *rows, int n_rows) {
    struct timeval start, end;
    gettimeofday(&start, NULL);
    printf("start : %d.%d\n", start.tv_sec, start.tv_usec);
    g_count = 0;

    for (int i = 0; i < n_rows; i++) {
        int a = rows[i].a;
        int b = rows[i].b;

        // 由于rows是递增的，因此 a > 3000 之后的行一定是要过滤掉的，直接跳出循环以提高性能
        if (a > 3000) {
            break;
        }

        g_count++;

        if ((b >= B_LOWER_LIMIT && b < B_UPPER_LIMIT) && (a == 1000 || a == 2000 || a == 3000)) {
            printf("%d,%d\n", a, b);
        } else {
            // 进入这个else分支，说明上面的条件不能满足，那么就说明a或b不满足条件
            int next_a = 0;
            // a不满足条件，那么就通过binary_search_a找到第一个满足要求的a的位置
            if (a < 1000) {
                next_a = 1000;
            }

            if (a > 1000 && a < 2000) {
                next_a = 2000;
            }

            if (a > 2000 && a < 3000) {
                next_a = 3000;
            }

            if (next_a != 0) { // next_a不是0，说明当前a值是不满足条件，需要找到第一个满足要求的a的位置
                int pos = binary_search_first_a(rows, i, n_rows - 1, next_a);
                i = pos - 1; // 出本次循环时i会加一，所以这里先减一
            } else { // a是满足条件的，b不满足条件
                // 如果b已经超过了上限，而且b是排序的，那么直接跳转到下一个a值
                if (b >= B_UPPER_LIMIT) {
                    if (a == 1000) {
                        next_a = 2000;
                    }

                    if (a == 2000) {
                        next_a = 3000;
                    }

                    int pos = binary_search_first_a(rows, i, n_rows - 1, next_a);
                    i = pos - 1; // 出本次循环时i会加一，所以这里先减一
                } else {
                    int pos_last_a = binary_search_last_a(rows, i, n_rows - 1, a);
                    int pos = binary_search_b(rows, i, pos_last_a, B_LOWER_LIMIT);
                    i = pos - 1; // 出本次循环时i会加一，所以这里先减一
                }
            }
        }
    }
    gettimeofday(&end, NULL);
    printf("end   : %d.%d\n", end.tv_sec, end.tv_usec);
    printf("cost(us): %d, g_count:%d\n", end.tv_sec*1000000 + end.tv_usec - start.tv_sec*1000000 - start.tv_usec, g_count);
}

void min_heapify(RowArr **pp_row_arr, int i, int length)
{
    int smallest = i;
    while(smallest <= length -1)
    {
        int left =2*smallest +1;
        int right =2*smallest +2;
        int temp = smallest;
        if(left <= length -1 && pp_row_arr[left]->row[pp_row_arr[left]->cur].b < pp_row_arr[smallest]->row[pp_row_arr[smallest]->cur].b)
        {
            smallest = left;
        }

        if(right <= length -1 && pp_row_arr[right]->row[pp_row_arr[right]->cur].b < pp_row_arr[smallest]->row[pp_row_arr[smallest]->cur].b)
        {
            smallest = right;
        }


        if(smallest != temp)
        {
            RowArr* exchange = pp_row_arr[smallest];
            pp_row_arr[smallest] = pp_row_arr[temp];
            pp_row_arr[temp] = exchange;
        }
        else {
            break;
        }
    }
}

void build_min_heap(Row **rows, int length)
{
    int root = length/2-1;
    for(int i = root; i >=0; i--) {
        min_heapify(rows, i, length);
    }
}

/*
 * task3基于task2，task2主要实现的是快速定位到a跳变点，由于a相同时b是排好序的，所以将满足条件的数据按照b排序的问题就转变成了将多个有序数组
 * 合并排序的问题，由于task4是要扩展有效a值的，所以这里实现需要考虑到a值扩展之后的性能，因此task3计划采用堆来实现多路合并，因为堆排序可以实现
 * nlog(k)，n是数据条数，k是排序数组的个数，多个排序数组都提取第一个节点构成小顶堆，然后将提取小顶堆的根节点，
 *
 * */
void task3(const Row *rows, int n_rows) {
    int heap_size = 3;
    int arr_len1 = 0;
    int arr_len2 = 0;
    int arr_len3 = 0;
    int idx = 0;
    int count = 0;
    RowArr row_arr[3] = {};
    Row result[200] = {};

    struct timeval start, end;
    gettimeofday(&start, NULL);
    printf("start : %d.%d\n", start.tv_sec, start.tv_usec);
    g_count = 0;
    for (int i = 0; i < n_rows; i++) {
        int a = rows[i].a;
        int b = rows[i].b;

        // 由于rows是递增的，因此 a > 3000 之后的行一定是要过滤掉的，直接跳出循环以提高性能
        if (a > 3000) {
            break;
        }

        g_count++;

        if ((b >= B_LOWER_LIMIT && b < B_UPPER_LIMIT) && (a == 1000 || a == 2000 || a == 3000)) {
            //printf("%d,%d\n", a, b);
            if (a == 1000) {
                row_arr[a/1000 - 1].row[arr_len1++] = rows[i];
                row_arr[a/1000 - 1].len = arr_len1;
            }

            if (a == 2000) {
                row_arr[a/1000 - 1].row[arr_len2++] = rows[i];
                row_arr[a/1000 - 1].len = arr_len2;
            }

            if (a == 3000) {
                row_arr[a/1000 - 1].row[arr_len3++] = rows[i];
                row_arr[a/1000 - 1].len = arr_len3;
            }
        } else {
            // 进入这个else分支，说明上面的条件不能满足，那么就说明a或b不满足条件
            int next_a = 0;
            // a不满足条件，那么就通过binary_search_a找到第一个满足要求的a的位置
            if (a < 1000) {
                next_a = 1000;
            }

            if (a > 1000 && a < 2000) {
                next_a = 2000;
            }

            if (a > 2000 && a < 3000) {
                next_a = 3000;
            }

            if (next_a != 0) { // next_a不是0，说明当前a值是不满足条件，需要找到第一个满足要求的a的位置
                int pos = binary_search_first_a(rows, i, n_rows - 1, next_a);
                i = pos - 1; // 出本次循环时i会加一，所以这里先减一
            } else { // a是满足条件的，b不满足条件
                // 如果b已经超过了上限，而且b是排序的，那么直接跳转到下一个a值
                if (b >= B_UPPER_LIMIT) {
                    if (a == 1000) {
                        next_a = 2000;
                    }

                    if (a == 2000) {
                        next_a = 3000;
                    }

                    int pos = binary_search_first_a(rows, i, n_rows - 1, next_a);
                    i = pos - 1; // 出本次循环时i会加一，所以这里先减一
                } else {
                    int pos_last_a = binary_search_last_a(rows, i, n_rows - 1, a);
                    int pos = binary_search_b(rows, i, pos_last_a, B_LOWER_LIMIT);
                    i = pos - 1; // 出本次循环时i会加一，所以这里先减一
                }
            }
        }
    }

    // RowArr指针数组，用来存放3个有序arr的地址
    RowArr* arr_heap[3] = {&row_arr[0], &row_arr[1], &row_arr[2]};
    // 先对每个有序数组第一个元素组成的数组进行排序
    build_min_heap(arr_heap, heap_size);

    // 每次取arr_heap[0]的下一个Row，如果取完，
    // 则把堆末尾的元素补到arr_heap[0],由于有--K,所以堆的大小减一
    while(heap_size >0)
    {
        // 一直是从heap的顶取值
        result[count++] = arr_heap[0]->row[arr_heap[0]->cur];
        // 这里判断条件是arr_heap[0]->len - 1，防止arr_heap[0]->cur++之后越界
        if(arr_heap[0]->cur < arr_heap[0]->len - 1) {
            arr_heap[0]->cur++;
        }
        else {
            arr_heap[0] = arr_heap[--heap_size];
        }
        min_heapify(arr_heap, 0, heap_size);
    }

    for (int i = 0; i < count; i++) {
        printf("%d,%d\n", result[i].a, result[i].b);
    }

    gettimeofday(&end, NULL);
    printf("end   : %d.%d\n", end.tv_sec, end.tv_usec);
    printf("cost(us): %d, g_count:%d\n", end.tv_sec*1000000 + end.tv_usec - start.tv_sec*1000000 - start.tv_usec, g_count);
}

/*
 * 关于task4，其实在task3里面也已经提到了，task4就是在task3基础上扩充了有序数组的个数，例如从3个扩充到100个，我在task3里面使用了小顶堆，
 * 实现了多个有序数组的合并，时间复杂度是nlog(k)，n是数据条数，k是排序数组的个数，所以在k扩展的时候能够实现比较好的优化效果
 * */

int main() {
    int count = 100000;
    Row rows[100000] = {};

    // task1输入
    /* for (int i = 0; i < count; i++) {
        rows[i].a = (i%10 + 1) * 1000;
        rows[i].b = i%100;
    } */

    // task2 & task3输入
    for (int i = 0; i < count; i++) {
        rows[i].a = (i/1000) * 100;
        rows[i].b = i%1000;
    }

    //task1(rows, count);
    //task2(rows, count);
    task3(rows, count);
    return 0;
}


