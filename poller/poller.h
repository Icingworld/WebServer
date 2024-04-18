#ifndef _POLLER_H_
#define _POLLER_H_

#include <iostream>
#include <thread>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <errno.h>

#include "../log/logger.h"
#include "../threadpool/threadpool.h"

class Poller
{
private:
    int port;                               // 端口号
    int listenMax;                          // listen最多排队的socket数
    int eventsMax;                          // epoll最大事件数
    bool exit;                              // 通知线程退出
    std::thread listeningThread;            // 监听线程
    ThreadPool * threadPool;                // 线程池实例
    Logger * logger;

    void mainEvent();                       // 监听线程函数

public:
    Poller(ThreadPool * threadPool, Logger * logger);
    ~Poller();

    void setPort(int newPort);              // 设置端口
    void setListenMax(int newQueueMax);      // 设置最大排队数
    void setEventsMax(int newMaxEvents);    // 设置epoll最大事件数

    void launch();                          // 启动Poller
    void shutdown();                        // 关闭Poller
};

#endif