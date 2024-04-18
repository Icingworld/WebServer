#ifndef _LOGWORKER_H_
#define _LOGWORKER_H_

#include <thread>
#include <queue>
#include <cstring>
#include <iostream>
#include <fstream>
#include <mutex>
#include <condition_variable>

#include "logbuffer.h"

/**
 * @brief 日志后端处理
**/
class LogWorker
{
private:
    LogBuffer * buffer;                     // 后端缓冲区
    std::string filePath;                   // 日志文件路径
    std::ofstream outputFile;               // 输出文件流
    std::queue<LogBuffer *> queue;          // 缓冲区队列
    std::thread workingThread;              // 工作线程
    std::mutex mtx;                         // 互斥量，保护queue
    std::condition_variable cv_work;
    std::thread readingThread;              // 读取线程
    std::condition_variable * cv_read;
    bool exit;                              // 通知线程退出

    void work();                            // 工作线程函数
    void read();                            // 读取线程函数

public:
    LogWorker(LogBuffer * buffer, const std::string & filePath, std::condition_variable * cv_read);
    ~LogWorker();

    void launch();                          // 后端启动
    void shutdown();                        // 后端关闭
};

#endif