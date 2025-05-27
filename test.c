#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>

sem_t agentSemaphore;       // 表示供应商是否可以提供材料
sem_t tobaccoAndPaper;      // 表示有烟草和纸可用
sem_t tobaccoAndGlue;       // 表示有烟草和胶水可用
sem_t paperAndGlue;         // 表示有纸和胶水可用
sem_t smokerDone;           // 表示抽烟者已完成抽烟

// 初始化随机数生成器
void initRandom() {
    srand(time(NULL));
}

// 供应商线程函数
void* agent(void* arg) {
    while (1) {
        // 等待抽烟者完成抽烟
        sem_wait(&smokerDone);
        
        // 随机选择提供哪两种材料
        int choice = rand() % 3;
        
        switch (choice) {
            case 0:
                printf("供应商提供了烟草和纸\n");
                sem_post(&tobaccoAndPaper);
                break;
            case 1:
                printf("供应商提供了烟草和胶水\n");
                sem_post(&tobaccoAndGlue);
                break;
            case 2:
                printf("供应商提供了纸和胶水\n");
                sem_post(&paperAndGlue);
                break;
        }
        
        // 等待抽烟者获取材料
        sem_wait(&agentSemaphore);
    }
    return NULL;
}

// 有烟草的抽烟者线程函数
void* smokerWithTobacco(void* arg) {
    while (1) {
        // 等待纸和胶水
        sem_wait(&paperAndGlue);
        
        printf("有烟草的抽烟者正在卷烟并抽烟\n");
        sleep(1); // 模拟卷烟和抽烟的时间
        
        // 抽烟完成，通知供应商
        sem_post(&agentSemaphore);
        sem_post(&smokerDone);
    }
    return NULL;
}

// 有纸的抽烟者线程函数
void* smokerWithPaper(void* arg) {
    while (1) {
        // 等待烟草和胶水
        sem_wait(&tobaccoAndGlue);
        
        printf("有纸的抽烟者正在卷烟并抽烟\n");
        sleep(1); // 模拟卷烟和抽烟的时间
        
        // 抽烟完成，通知供应商
        sem_post(&agentSemaphore);
        sem_post(&smokerDone);
    }
    return NULL;
}

// 有胶水的抽烟者线程函数
void* smokerWithGlue(void* arg) {
    while (1) {
        // 等待烟草和纸
        sem_wait(&tobaccoAndPaper);
        
        printf("有胶水的抽烟者正在卷烟并抽烟\n");
        sleep(1); // 模拟卷烟和抽烟的时间
        
        // 抽烟完成，通知供应商
        sem_post(&agentSemaphore);
        sem_post(&smokerDone);
    }
    return NULL;
}

int main() {
    // 初始化信号量
    sem_init(&agentSemaphore, 0, 0);
    sem_init(&tobaccoAndPaper, 0, 0);
    sem_init(&tobaccoAndGlue, 0, 0);
    sem_init(&paperAndGlue, 0, 0);
    sem_init(&smokerDone, 0, 1); // 初始时允许供应商提供材料
    
    // 初始化随机数生成器
    initRandom();
    
    // 创建线程
    pthread_t agentThread, smoker1, smoker2, smoker3;
    
    pthread_create(&agentThread, NULL, agent, NULL);
    pthread_create(&smoker1, NULL, smokerWithTobacco, NULL);
    pthread_create(&smoker2, NULL, smokerWithPaper, NULL);
    pthread_create(&smoker3, NULL, smokerWithGlue, NULL);
    
    // 等待线程结束（这里不会结束，因为是无限循环）
    pthread_join(agentThread, NULL);
    pthread_join(smoker1, NULL);
    pthread_join(smoker2, NULL);
    pthread_join(smoker3, NULL);
    
    // 销毁信号量
    sem_destroy(&agentSemaphore);
    sem_destroy(&tobaccoAndPaper);
    sem_destroy(&tobaccoAndGlue);
    sem_destroy(&paperAndGlue);
    sem_destroy(&smokerDone);
    
    return 0;
}
