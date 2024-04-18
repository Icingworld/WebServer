#include "threadpool.h"

ThreadPool::ThreadPool(int size, Logger * logger)
    : size(size)
    , exit(false)
    , logger(logger)
{

}

ThreadPool::~ThreadPool()
{

}

Thread * ThreadPool::createNewThread()
{
    Thread * newThread = new Thread(&task, &mtx, &condition, &exit, logger);
    pool.emplace_back(newThread);
    return newThread;
}

void ThreadPool::appendTask(int fd)
{
    std::unique_lock<std::mutex> lock(mtx);
    task.emplace(fd);
    lock.unlock();
    // 通知一个线程
    condition.notify_one();
}

void ThreadPool::launch()
{
    exit = false;
    // 初始化线程池
    for (int i=0;i<size;i++) {
        createNewThread();
    }
    
    // 启动线程池所有线程
    for (const auto & thread : pool) {
        thread->launch();
    }
}

void ThreadPool::shutdown()
{
    exit = true;
    condition.notify_all();     // 唤醒所有线程，并通知退出

    // 删除线程池
    for (const auto & toDel : pool) {
        delete toDel;
    }
}

void ThreadPool::setEventsMax(int newEventsMax)
{
    for (const auto & thread : pool) {
        thread->setEventsMax(newEventsMax);
    }
}
