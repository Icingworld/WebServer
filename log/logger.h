#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <unistd.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>

#include "logbuffer.h"
#include "logworker.h"

/**
 * @brief 日志类
 * @public log();
*/
class Logger
{
private:
    size_t size;
    LogBuffer bufferA;                          // 前端缓冲区
    LogBuffer bufferB;                          // 后端缓冲区
    std::string filePath;                       // 日志文件名
    LogWorker * worker;                         // logworker实例
    bool exit;                                  // 通知线程退出
    int timeout;                                // 定时时长
    std::thread timerThread;                    // 定时器线程
    std::condition_variable cv_worker;          // logworker的条件量

    void timer();                               // 定时器函数
    void swap();                                // 交换缓冲区
    std::string getTime();                      // 获取当前时间

public:
    enum LEVEL {
        INFO,
        WARNING,
        ERROR,
        FATAL
    };

    Logger(const std::string & filePath);
    ~Logger();

    void launch();                                      // 启动logger
    void shutdown();                                    // 关闭logger
    void log(const char * message, int level=INFO);     // 日志记录
};

#endif