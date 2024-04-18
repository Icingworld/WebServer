#ifndef _THREAD_H_
#define _THREAD_H_

#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <unistd.h>
#include <sys/epoll.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdlib>

#include "../global_define.h"
#include "../http/httphandler.h"
#include "../log/logger.h"

class Thread
{
private:
    std::queue<int> * task;                 // 任务队列
    std::mutex * mtx;                       // 互斥量
    std::condition_variable * condition;    // 线程条件变量
    bool * exit;                            // 通知线程退出
    std::thread readingTaskThread;          // 读任务线程
    std::thread handlingThread;             // epoll读取线程
    int epoll_fd;                           // epoll描述符
    int eventsMax;                          // 最大处理事件数
    HttpHandler http;                       // 请求解析器
    Logger * logger;                        // 日志

    void getTask();                         // 读任务线程函数
    void handle();                          // epoll读取线程函数

public:
    Thread(std::queue<int> * task, std::mutex * mtx, std::condition_variable * condition, bool * exit, Logger * logger);
    ~Thread();

    void launch();
    void setEventsMax(int newEventsMax);
};

#endif