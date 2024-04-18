#include "logworker.h"

LogWorker::LogWorker(LogBuffer * buffer, const std::string & filePath, std::condition_variable * cv_read)
    : buffer(buffer)
    , filePath(filePath)
    , exit(false)
    , cv_read(cv_read)
{
    
}

LogWorker::~LogWorker()
{

}

void LogWorker::launch()
{
    exit = false;
    // 启动线程
    readingThread = std::thread(&LogWorker::read, this);
    workingThread = std::thread(&LogWorker::work, this);
    readingThread.detach();
    workingThread.detach();
}

void LogWorker::shutdown()
{
    exit = true;
    cv_work.notify_one();
    cv_read->notify_one();
}

/**
 * @brief 从队列中取出全部缓冲区写入磁盘
**/
void LogWorker::work()
{
    while (!exit) {
        std::unique_lock<std::mutex> lock(mtx);
        if (!queue.empty()) {
            // 取出一个缓冲区并写入文件
            LogBuffer * tempBuffer = queue.front();
            outputFile = std::ofstream(filePath, std::ios::app);
            if (!outputFile.is_open()) {
                printf("failed to write log file\n");
            }
            outputFile << tempBuffer->data();
            outputFile.close();
            queue.pop();
            delete tempBuffer;
        } else {
            cv_work.wait(lock);
        }
    }
}

/**
 * @brief 将缓冲区复制到队列中
 * @todo 队列太长就删除
**/
void LogWorker::read()
{
    while (!exit)
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv_read->wait(lock);
        if (!buffer->isEmpty()) {
            // 复制并添加到队列中
            LogBuffer * tempBuffer = new LogBuffer(buffer);
            queue.emplace(tempBuffer);
            // printf("queue length: %zu\n", queue.size());
            // 清空缓冲区
            buffer->reset();
            // 唤醒work线程写入
            cv_work.notify_one();
        }
    }
}