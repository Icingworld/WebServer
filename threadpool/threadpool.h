#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>

#include "thread.h"

/**
 * @brief 线程池
*/
class ThreadPool
{
private:
    int size;                           // 线程池大小
    std::vector<Thread *> pool;         // 线程池容器
    std::queue<int> task;               // 任务队列，储存client_fd
    std::mutex mtx;                     // 互斥量，用于保护task队列
    std::condition_variable condition;  // 条件量
    bool exit;                          // 通知线程结束
    Logger * logger;

    Thread * createNewThread();         // 创建新线程
    
public:
    ThreadPool(int size, Logger * logger);
    ~ThreadPool();

    void appendTask(int fd);            // 添加client_fd
    void launch();                      // 启动线程池
    void shutdown();                    // 关闭线程池

    void setEventsMax(int newEventsMax);
};

#endif