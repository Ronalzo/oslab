#include "vmrp.h"
#include <cstring>
#include <ctime>
#include <random>
#include <vector>
#include <climits>
#include <unordered_map>
using namespace std;
// 构造函数
Replace::Replace() {
    char choice;
    std::cout << "(y/n): ";
    std::cin >> choice;
    
    if (choice == 'y' || choice == 'Y') {
        int max_page;
        std::cout << "Enter max page number (e.g. 10): ";
        std::cin >> max_page;
        
        std::cout << "Enter sequence length: ";
        std::cin >> PageNumber;
        
        ReferencePage = new int[PageNumber];
        EliminatePage = new int[PageNumber];
        
        srand(time(0));
        std::cout << "Generated reference: ";
        for (int i = 0; i < PageNumber; i++) {
            ReferencePage[i] = rand() % max_page;
            std::cout << ReferencePage[i] << " ";
        }
        std::cout << "\n";
    } else {
        // 原始输入逻辑
        std::cout << "Please input page numbers: ";
        std::cin >> PageNumber;
        ReferencePage = new int[PageNumber];
        EliminatePage = new int[PageNumber];

        std::cout << "Please input reference page string: ";
        for (int i = 0; i < PageNumber; i++) {
            std::cin >> ReferencePage[i];
        }
    }

    std::cout << "Please input page frames: ";
    std::cin >> FrameNumber;
    PageFrames = new int[FrameNumber];
    elim_index = 0;
}

// 析构函数
Replace::~Replace() {
    delete[] ReferencePage;
    delete[] EliminatePage;
    delete[] PageFrames;
}

// 初始化空间
void Replace::InitSpace(const char *MethodName) {
    std::cout << "\n" << MethodName << "\n";
    FaultNumber = 0;
    elim_index = 0;
    for (int i = 0; i < PageNumber; i++) EliminatePage[i] = -1;
    for (int i = 0; i < FrameNumber; i++) PageFrames[i] = -1;
}

// 报告结果
void Replace::Report(void) {
    std::cout << "\nEliminate page:";
    for (int i = 0; i < elim_index; i++) 
        std::cout << EliminatePage[i] << " ";
    
    std::cout << "\nNumber of page faults = " << FaultNumber;
    std::cout << "\nRate of page faults = " 
              << std::fixed << std::setprecision(1)
              << (100.0 * FaultNumber / PageNumber) << "%\n";
}

// LRU算法实现
void Replace::Lru(void) {
    InitSpace("LRU");
    int *stack = new int[FrameNumber]; // LRU栈

    for (int k = 0; k < PageNumber; k++) {
        int next = ReferencePage[k];
        bool found = false;
        int pos = 0;
        // 检查是否在内存中
        for (int i = 0; i < FrameNumber; i++) {
            if (PageFrames[i] == next) {
                found = true;
                pos = i;
                break;
            }
        }

        // 页面命中
        if (found) {
            // 更新LRU栈：将命中页移到栈顶
            int temp = stack[pos];
            for (int j = pos; j > 0; j--) {
                stack[j] = stack[j - 1];
                PageFrames[j] = PageFrames[j - 1];
            }
            stack[0] = temp;
            PageFrames[0] = next;
        } 
        // 缺页处理
        else {
            FaultNumber++;
            // 栈满时淘汰栈底页面
            if (PageFrames[FrameNumber - 1] != -1) {
                EliminatePage[elim_index++] = PageFrames[FrameNumber - 1];
            }

            // 整体下移
            for (int j = FrameNumber - 1; j > 0; j--) {
                stack[j] = stack[j - 1];
                PageFrames[j] = PageFrames[j - 1];
            }
            
            // 新页放入栈顶
            stack[0] = next;
            PageFrames[0] = next;
        }

        // 输出当前状态
        for (int j = 0; j < FrameNumber; j++) {
            if (PageFrames[j] != -1) std::cout << PageFrames[j] << " ";
        }
        if (!found && elim_index > 0) 
            std::cout << "->" << EliminatePage[elim_index - 1];
        std::cout << "\n";
    }
    delete[] stack;
    Report();
}

// FIFO算法实现
void Replace::Fifo(void) {
    InitSpace("FIFO");
    int pointer = 0; // FIFO指针

    for (int k = 0; k < PageNumber; k++) {
        int next = ReferencePage[k];
        bool found = false;

        // 检查是否在内存中
        for (int i = 0; i < FrameNumber; i++) {
            if (PageFrames[i] == next) {
                found = true;
                break;
            }
        }

        // 缺页处理
        if (!found) {
            FaultNumber++;
            // 记录淘汰页
            if (PageFrames[pointer] != -1) {
                EliminatePage[elim_index++] = PageFrames[pointer];
            }
            
            // 替换页面
            PageFrames[pointer] = next;
            pointer = (pointer + 1) % FrameNumber;
        }

        // 输出当前状态
        for (int i = 0; i < FrameNumber; i++) {
            if (PageFrames[i] != -1) std::cout << PageFrames[i] << " ";
        }
        if (!found && elim_index > 0) 
            std::cout << "->" << EliminatePage[elim_index - 1];
        std::cout << "\n";
    }
    Report();
}


//增强二次机会算法
void Replace::Eclock(void) {
    InitSpace("ECLOCK");
    ref_bits = new int[FrameNumber](); // 初始化为0
    modify_bits = new int[FrameNumber](); // 初始化为0
    clock_hand = 0;

    for (int k = 0; k < PageNumber; k++) {
        int next = ReferencePage[k];
        bool found = false;
        int replace_index = -1;

        // 检查页面是否在内存中
        for (int i = 0; i < FrameNumber; i++) {
            if (PageFrames[i] == next) {
                found = true;
                ref_bits[i] = 1; // 设置使用位
                 modify_bits[i] = 1; // 如果是写操作设置修改位
                break;
            }
        }

        // 输出当前状态
        for (int i = 0; i < FrameNumber; i++) {
            if (PageFrames[i] != -1) {
                std::cout << PageFrames[i] << "(" 
                          << ref_bits[i] << "," 
                          << modify_bits[i] << ") ";
            }
        }

        if (found) {
            std::cout << "\n";
            continue;
        }

        // 缺页处理
        FaultNumber++;
        int start_hand = clock_hand;
        bool found_candidate = false;

        // 查找可替换页面 (最多扫描两轮)
        for (int scan = 0; scan < 2 * FrameNumber; scan++) {
            int idx = (clock_hand + scan) % FrameNumber;
            
            // 跳过空帧
            if (PageFrames[idx] == -1) {
                replace_index = idx;
                found_candidate = true;
                break;
            }
            
            // 检查 (ref, mod) 组合
            if (ref_bits[idx] == 0) {
                if (modify_bits[idx] == 0) { // (0,0) 理想候选
                    replace_index = idx;
                    found_candidate = true;
                    break;
                } else if (replace_index == -1) { // (0,1) 备选
                    replace_index = idx;
                }
            } else { // 给第二次机会
                ref_bits[idx] = 0;
            }
        }

        // 确定替换页面
        if (!found_candidate && replace_index == -1) {
            replace_index = clock_hand; // 默认替换当前指针位置
        }

        // 记录淘汰页
        if (PageFrames[replace_index] != -1) {
            EliminatePage[elim_index++] = PageFrames[replace_index];
            std::cout << " -> " << PageFrames[replace_index];
        }

        // 加载新页面
        PageFrames[replace_index] = next;
        ref_bits[replace_index] = 1;
        // modify_bits[replace_index] = (rand() % 2); // 随机设置修改位
        
        // 移动时钟指针
        clock_hand = (replace_index + 1) % FrameNumber;
        
        std::cout << "\n";
    }
    
    delete[] ref_bits;
    delete[] modify_bits;
    Report();
}
// 其他算法空实现
void Replace::Clock(void) {
      InitSpace("Clock");
    int *ref_bits = new int[FrameNumber](); // 引用位数组
    int pointer = 0; // 时钟指针

    for (int k = 0; k < PageNumber; k++) {
        int next = ReferencePage[k];
        bool found = false;
        int free_slot = -1;

        // 检查是否在内存中
        for (int i = 0; i < FrameNumber; i++) {
            if (PageFrames[i] == next) {
                found = true;
                ref_bits[i] = 1; // 设置引用位
                break;
            }
            if (free_slot == -1 && PageFrames[i] == -1) {
                free_slot = i; // 记录空闲位置
            }
        }

        // 缺页处理
        if (!found) {
            FaultNumber++;
            // 如果有空闲帧，直接使用
            if (free_slot != -1) {
                PageFrames[free_slot] = next;
                ref_bits[free_slot] = 1;
            } else {
                // 寻找淘汰页面
                while (true) {
                    if (ref_bits[pointer] == 0) {
                        // 淘汰页面
                        EliminatePage[elim_index++] = PageFrames[pointer];
                        PageFrames[pointer] = next;
                        ref_bits[pointer] = 1;
                        pointer = (pointer + 1) % FrameNumber;
                        break;
                    } else {
                        // 给第二次机会
                        ref_bits[pointer] = 0;
                        pointer = (pointer + 1) % FrameNumber;
                    }
                }
            }
        }

        // 输出当前状态
        for (int i = 0; i < FrameNumber; i++) {
            if (PageFrames[i] != -1) 
                std::cout << PageFrames[i] << (ref_bits[i] ? "* " : " ");
        }
        if (!found && elim_index > 0) 
            std::cout << "->" << EliminatePage[elim_index - 1];
        std::cout << "\n";
    }
    delete[] ref_bits;
    Report();
}
void Replace::Lfu(void) {
    InitSpace("LFU");
    int *counters = new int[FrameNumber](); // 访问计数器
    int *ages = new int[FrameNumber](); // 页面年龄

    for (int k = 0; k < PageNumber; k++) {
        int next = ReferencePage[k];
        bool found = false;
        int free_slot = -1;

        // 检查是否在内存中
        for (int i = 0; i < FrameNumber; i++) {
            if (PageFrames[i] == next) {
                found = true;
                counters[i]++; // 增加访问计数
                break;
            }
            if (free_slot == -1 && PageFrames[i] == -1) {
                free_slot = i; // 记录空闲位置
            }
            ages[i]++; // 增加页面年龄
        }

        // 缺页处理
        if (!found) {
            FaultNumber++;
            // 如果有空闲帧，直接使用
            if (free_slot != -1) {
                PageFrames[free_slot] = next;
                counters[free_slot] = 1;
                ages[free_slot] = 0;
            } else {
                // 寻找使用频率最低的页面
                int min_counter = INT_MAX;
                int min_age = -1;
                int victim = -1;
                
                for (int i = 0; i < FrameNumber; i++) {
                    if (counters[i] < min_counter || 
                       (counters[i] == min_counter && ages[i] > min_age)) {
                        min_counter = counters[i];
                        min_age = ages[i];
                        victim = i;
                    }
                }
                
                // 淘汰页面
                EliminatePage[elim_index++] = PageFrames[victim];
                PageFrames[victim] = next;
                counters[victim] = 1;
                ages[victim] = 0;
            }
        }

        // 输出当前状态
        for (int i = 0; i < FrameNumber; i++) {
            if (PageFrames[i] != -1) 
                std::cout << PageFrames[i] << "(" << counters[i] << ") ";
        }
        if (!found && elim_index > 0) 
            std::cout << "->" << EliminatePage[elim_index - 1];
        std::cout << "\n";
    }
    delete[] counters;
    delete[] ages;
    Report();
}
void Replace::Mfu(void) {
    InitSpace("Most Frequently Used (MFU)");
    
    // 页面访问频率计数器
    unordered_map<int, int> frequency;
    // 页面进入时间（用于处理频率相同的情况）
    unordered_map<int, int> entryTime;
    int timeCounter = 0;
    
    for (int i = 0; i < PageNumber; i++) {
        int page = ReferencePage[i];
        frequency[page]++;  // 更新访问频率
        
        bool found = false;
        int pagePos = -1;
        
        // 检查页面是否已在内存
        for (int j = 0; j < FrameNumber; j++) {
            if (PageFrames[j] == page) {
                found = true;
                pagePos = j;
                break;
            }
        }
        
        if (found) {
            // 输出当前内存状态
            for (int j = 0; j < FrameNumber; j++) {
                if (PageFrames[j] != -1)
                    cout << PageFrames[j] << "(" << frequency[PageFrames[j]] << ") ";
                else
                    cout << "- ";
            }
            cout << endl;
            timeCounter++;
            continue;
        }
        
        // 页面错误
        FaultNumber++;
        
        // 检查是否有空闲帧
        bool hasFreeFrame = false;
        for (int j = 0; j < FrameNumber; j++) {
            if (PageFrames[j] == -1) {
                PageFrames[j] = page;
                entryTime[page] = timeCounter++;
                hasFreeFrame = true;
                break;
            }
        }
        
        if (!hasFreeFrame) {
            // 寻找使用频率最高的页面（频率相同时选择最早进入的）
            int mfuPage = -1;
            int maxFreq = -1;
            int minTime = INT_MAX;
            int replaceIdx = -1;
            
            for (int j = 0; j < FrameNumber; j++) {
                int currPage = PageFrames[j];
                if (frequency[currPage] > maxFreq || 
                   (frequency[currPage] == maxFreq && entryTime[currPage] < minTime)) {
                    maxFreq = frequency[currPage];
                    minTime = entryTime[currPage];
                    mfuPage = currPage;
                    replaceIdx = j;
                }
            }
            
            EliminatePage[i] = PageFrames[replaceIdx];  // 记录被淘汰的页面
            PageFrames[replaceIdx] = page;
            entryTime[page] = timeCounter++;
        }
        
        // 输出当前内存状态
        for (int j = 0; j < FrameNumber; j++) {
            if (PageFrames[j] != -1)
                cout << PageFrames[j] << "(" << frequency[PageFrames[j]] << ") ";
            else
                cout << "- ";
        }
        if (EliminatePage[i] != -1)
            cout << "-> " << EliminatePage[i];
        cout << endl;
    }
    
    Report();
}
void Replace::CompareAlgorithms() {
    int frames[] = {1, 2, 3, 4, 5, 6};
    int num_tests = sizeof(frames)/sizeof(frames[0]);
    
    std::cout << "\nPerformance Comparison (Page Fault Rates)\n";
    std::cout << "Frames\tFIFO\tLRU\tClock\tEClock\tLFU\tMFU\n";
    std::cout << "--------------------------------------------------\n";
    
    // 保存原始帧数
    int orig_frames = FrameNumber;
    
    for (int i = 0; i < num_tests; i++) {
        FrameNumber = frames[i];
        
        // 测试 FIFO
        InitSpace("");
        Fifo();
        float fifo_rate = 100.0 * FaultNumber / PageNumber;
        
        // 测试 LRU
        InitSpace("");
        Lru();
        float lru_rate = 100.0 * FaultNumber / PageNumber;
        
        // 测试 Clock
        InitSpace("");
        Clock();
        float clock_rate = 100.0 * FaultNumber / PageNumber;
        
        // 测试 EClock
        InitSpace("");
        Eclock();
        float eclock_rate = 100.0 * FaultNumber / PageNumber;
        
        // 测试 LFU
        InitSpace("");
        Lfu();
        float lfu_rate = 100.0 * FaultNumber / PageNumber;
        
        // 测试 MFU
        InitSpace("");
        Mfu();
        float mfu_rate = 100.0 * FaultNumber / PageNumber;
        
        printf("%d\t%.1f%%\t%.1f%%\t%.1f%%\t%.1f%%\t%.1f%%\t%.1f%%\n", 
               frames[i], fifo_rate, lru_rate, clock_rate, 
               eclock_rate, lfu_rate, mfu_rate);
    }
    
    // 恢复原始帧数
    FrameNumber = orig_frames;
}
// 主函数
int main() {
    Replace *vmpr = new Replace();
    vmpr->Fifo();
    vmpr->Lru();
    vmpr->Clock();
    vmpr->Eclock();
    vmpr->Lfu();
    vmpr->Mfu();
    char choice;
    std::cout << "\nRun performance comparison? (y/n): ";
    std::cin >> choice;
    if (choice == 'y' || choice == 'Y') {
        vmpr->CompareAlgorithms();
    }
    
    delete vmpr;
    return 0;
}
