#ifndef VMRP_H
#define VMRP_H

#include <iostream>
#include <iomanip>
#include <malloc.h>

class Replace {
public:
    Replace();
    ~Replace();
    void InitSpace(const char *MethodName);
    void Report(void);
    void Fifo(void);
    void Lru(void);
    void Clock(void);
    void Eclock(void);
    void Lfu(void);
    void Mfu(void);
    void CompareAlgorithms();
private:
    int *ReferencePage;  // 页面引用串
    int *EliminatePage;  // 淘汰页号记录
    int *PageFrames;     // 内存页帧
    int PageNumber;      // 总页数
    int FrameNumber;     // 内存帧数
    int FaultNumber;     // 缺页次数
    int elim_index;      // 淘汰记录索引 (新增)

    // 独立实验
    int* ref_bits;    // 使用位数组
    int* modify_bits; // 修改位数组 (可选)
    int clock_hand;   // 时钟指针
};

#endif