#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// 计算阶乘 f(x)
int factorial(int x) {
    if (x == 0 || x == 1) return 1;
    int result = 1;
    for (int i = 2; i <= x; i++) {
        result *= i;
    }
    return result;
}

// 计算斐波那契数 f(y)
int fibonacci(int y) {
    if (y == 1 || y == 2) return 1;
    int a = 1, b = 1, c;
    for (int i = 3; i <= y; i++) {
        c = a + b;
        a = b;
        b = c;
    }
    return b;
}

int main() {
    int x = 3, y = 4; // 可修改为命令行参数输入
    int pipe1[2], pipe2[2]; // 管道1用于f(x)通信，管道2用于f(y)通信

    // 创建管道
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Pipe creation failed");
        return EXIT_FAILURE;
    }

    // 子进程1：计算f(x)并通过管道1发送结果
    pid_t pid1 = fork();
    if (pid1 == 0) {
        close(pipe1[0]); // 关闭读端
        int result_x = factorial(x);
        write(pipe1[1], &result_x, sizeof(int)); // 写入结果
        close(pipe1[1]); // 关闭写端
        exit(EXIT_SUCCESS);
    } else if (pid1 < 0) {
        perror("Fork failed for f(x) process");
        return EXIT_FAILURE;
    }

    // 子进程2：计算f(y)并通过管道2发送结果
    pid_t pid2 = fork();
    if (pid2 == 0) {
        close(pipe2[0]); // 关闭读端
        int result_y = fibonacci(y);
        write(pipe2[1], &result_y, sizeof(int)); // 写入结果
        close(pipe2[1]); // 关闭写端
        exit(EXIT_SUCCESS);
    } else if (pid2 < 0) {
        perror("Fork failed for f(y) process");
        return EXIT_FAILURE;
    }

    // 父进程：关闭写端，读取结果并计算f(x,y)
    close(pipe1[1]);
    close(pipe2[1]);

    int rx, ry;
    read(pipe1[0], &rx, sizeof(int)); // 读取f(x)结果
    read(pipe2[0], &ry, sizeof(int)); // 读取f(y)结果

    int result = rx + ry;
    printf("f(%d, %d) = f(%d) + f(%d) = %d + %d = %d\n",
           x, y, x, y, rx, ry, result);

    // 关闭读端并等待子进程结束
    close(pipe1[0]);
    close(pipe2[0]);
    wait(NULL); // 等待子进程1
    wait(NULL); // 等待子进程2

    return EXIT_SUCCESS;
}