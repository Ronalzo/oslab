#include "dask.h"

// 构造函数，获取用户输入的初始磁道、方向和请求序列
DiskArm::DiskArm() {
    std::cout << "Please input current Cylinder:";
    std::cin >> CurrentCylinder;
    
    // 磁头方向说明
    std::cout << "磁头方向说明：0 表示向小磁道号方向移动，1 表示向大磁道号方向移动" << std::endl;
    std::cout << "Please input current direction (0/1):";
    std::cin >> SeekDirection;
    
    // 输入磁盘请求数和请求道号
    std::cout << "Please input request numbers:";
    std::cin >> RequestNumber;
    std::cout << "Please input request cylinder string (以空格分隔):";
    
    Request = new int[RequestNumber];
    Cylinder = new int[RequestNumber];
    for (int i = 0; i < RequestNumber; i++) {
        std::cin >> Request[i];
        Cylinder[i] = Request[i];
    }
}

// 析构函数，释放动态分配的内存
DiskArm::~DiskArm() {
    delete[] Request;
    delete[] Cylinder;
}

// 初始化道号和寻道记录
void DiskArm::InitSpace(char* MethodName) {
    std::cout << std::endl << "==== " << MethodName << " 算法执行过程 ====" << std::endl;
    SeekNumber = 0;
    SeekChang = 0;
    for (int i = 0; i < RequestNumber; i++) {
        Cylinder[i] = Request[i];
    }
}

// 统计报告算法执行情况
void DiskArm::Report() {
    std::cout << std::endl;
    std::cout << "总寻道距离: " << SeekNumber << std::endl;
    std::cout << "磁头调头次数: " << SeekChang << std::endl << std::endl;
}

// FCFS算法：先来先服务
void DiskArm::FCFS() {
    InitSpace("FCFS");
    int current = CurrentCylinder;
    int direction = SeekDirection;
    
    std::cout << "初始磁头位置: " << current;
    
    for (int i = 0; i < RequestNumber; i++) {
        bool needChangeDirection = ((Cylinder[i] >= current) && !direction) || 
                                   ((Cylinder[i] < current) && direction);
        
        if (needChangeDirection) {
            direction = !direction;
            SeekChang++;
            std::cout << std::endl << current << " -> " << Cylinder[i];
        } else {
            std::cout << " -> " << Cylinder[i];
            SeekNumber += abs(current - Cylinder[i]);
            current = Cylinder[i];
        }
    }
    
    Report();
}

// SSTF算法：最短寻道时间优先
void DiskArm::SSTF() {
    InitSpace("SSTF");
    int current = CurrentCylinder;
    std::vector<int> requests(Request, Request + RequestNumber);
    std::vector<bool> visited(RequestNumber, false);
    
    std::cout << "初始磁头位置: " << current;
    
    for (int i = 0; i < RequestNumber; i++) {
        int minSeek = INT_MAX;
        int nextCylinder = current;
        int minIndex = -1;
        
        for (int j = 0; j < RequestNumber; j++) {
            if (!visited[j]) {
                int seekDistance = abs(requests[j] - current);
                if (seekDistance < minSeek) {
                    minSeek = seekDistance;
                    nextCylinder = requests[j];
                    minIndex = j;
                }
            }
        }
        
        visited[minIndex] = true;
        std::cout << " -> " << nextCylinder;
        SeekNumber += minSeek;
        current = nextCylinder;
    }
    
    Report();
}

// SCAN算法：扫描算法（电梯算法）
void DiskArm::SCAN() {
    InitSpace("SCAN");
    int current = CurrentCylinder;
    int direction = SeekDirection;
    std::vector<int> requests(Request, Request + RequestNumber);
    std::sort(requests.begin(), requests.end());
    
    // 假设磁盘边界为0和200
    const int MIN_CYLINDER = 0;
    const int MAX_CYLINDER = 200;
    
    // 找到当前磁头在排序后请求中的位置
    auto it = std::lower_bound(requests.begin(), requests.end(), current);
    int pos = std::distance(requests.begin(), it);
    
    std::cout << "初始磁头位置: " << current;
    
    if (direction == 1) { // 向大磁道方向移动
        // 处理当前位置到最大磁道的所有请求
        for (int i = pos; i < requests.size(); i++) {
            std::cout << " -> " << requests[i];
            SeekNumber += abs(current - requests[i]);
            current = requests[i];
        }
        // 移动到最大磁道边界
        if (current < MAX_CYLINDER) {
            SeekNumber += (MAX_CYLINDER - current);
            current = MAX_CYLINDER;
        }
        SeekChang++; // 调头
        
        // 反向处理剩余请求
        for (int i = pos - 1; i >= 0; i--) {
            std::cout << " -> " << requests[i];
            SeekNumber += abs(current - requests[i]);
            current = requests[i];
        }
    } else { // 向小磁道方向移动
        // 处理当前位置到最小磁道的所有请求
        for (int i = pos; i >= 0; i--) {
            std::cout << " -> " << requests[i];
            SeekNumber += abs(current - requests[i]);
            current = requests[i];
        }
        // 移动到最小磁道边界
        if (current > MIN_CYLINDER) {
            SeekNumber += (current - MIN_CYLINDER);
            current = MIN_CYLINDER;
        }
        SeekChang++; // 调头
        
        // 反向处理剩余请求
        for (int i = pos + 1; i < requests.size(); i++) {
            std::cout << " -> " << requests[i];
            SeekNumber += abs(current - requests[i]);
            current = requests[i];
        }
    }
    
    Report();
}

// CSCAN算法：循环扫描算法
void DiskArm::CSCAN() {
    InitSpace("CSCAN");
    int current = CurrentCylinder;
    int direction = SeekDirection;
    std::vector<int> requests(Request, Request + RequestNumber);
    std::sort(requests.begin(), requests.end());
    
    // 获取磁盘边界
    int minCyl = *std::min_element(requests.begin(), requests.end());
    int maxCyl = *std::max_element(requests.begin(), requests.end());
    
    // 找到当前磁头在排序后请求中的位置
    auto it = std::lower_bound(requests.begin(), requests.end(), current);
    int pos = std::distance(requests.begin(), it);
    
    std::cout << "初始磁头位置: " << current;
    
    if (direction == 1) { // 向大磁道方向移动
        // 处理当前位置到最大磁道的所有请求
        for (int i = pos; i < requests.size(); i++) {
            std::cout << " -> " << requests[i];
            SeekNumber += abs(current - requests[i]);
            current = requests[i];
        }
        // 移动到最大磁道
        if (current < maxCyl) {
            SeekNumber += (maxCyl - current);
            current = maxCyl;
        }
        SeekChang++; // 调头
        
        // 跳转到最小磁道（不处理中间请求）
        SeekNumber += (maxCyl - minCyl);
        current = minCyl;
        
        // 处理最小磁道到当前位置左侧的请求
        for (int i = 0; i < pos; i++) {
            std::cout << " -> " << requests[i];
            SeekNumber += abs(current - requests[i]);
            current = requests[i];
        }
    } else { // 向小磁道方向移动，转换为大磁道方向处理
        direction = 1;
        std::cout << "CSCAN默认方向为大磁道，已调整方向..." << std::endl;
        InitSpace("CSCAN");
        CSCAN();
    }
    
    Report();
}

// LOOK算法：优化的SCAN算法，不到达磁盘边界
void DiskArm::LOOK() {
    InitSpace("LOOK");
    int current = CurrentCylinder;
    int direction = SeekDirection;
    std::vector<int> requests(Request, Request + RequestNumber);
    std::sort(requests.begin(), requests.end());
    
    // 找到当前磁头在排序后请求中的位置
    auto it = std::lower_bound(requests.begin(), requests.end(), current);
    int pos = std::distance(requests.begin(), it);
    
    std::cout << "初始磁头位置: " << current;
    
    if (direction == 1) { // 向大磁道方向移动
        // 处理当前位置到最大请求的所有请求
        for (int i = pos; i < requests.size(); i++) {
            std::cout << " -> " << requests[i];
            SeekNumber += abs(current - requests[i]);
            current = requests[i];
        }
        // 到达最大请求后调头
        if (pos > 0) {
            SeekChang++;
        }
        
        // 反向处理剩余请求
        for (int i = pos - 1; i >= 0; i--) {
            std::cout << " -> " << requests[i];
            SeekNumber += abs(current - requests[i]);
            current = requests[i];
        }
    } else { // 向小磁道方向移动
        // 处理当前位置到最小请求的所有请求
        for (int i = pos; i >= 0; i--) {
            std::cout << " -> " << requests[i];
            SeekNumber += abs(current - requests[i]);
            current = requests[i];
        }
        // 到达最小请求后调头
        if (pos < requests.size() - 1) {
            SeekChang++;
        }
        
        // 反向处理剩余请求
        for (int i = pos + 1; i < requests.size(); i++) {
            std::cout << " -> " << requests[i];
            SeekNumber += abs(current - requests[i]);
            current = requests[i];
        }
    }
    
    Report();
}