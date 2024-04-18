#include "logger.h"

Logger::Logger(const std::string & filePath)
    : bufferA()
    , bufferB()
    , filePath(filePath)
    , worker(new LogWorker(&bufferB, filePath, &cv_worker))
    , exit(false)
    , timeout(DEFAULT_SWAP_TIME)
{

}

Logger::~Logger()
{
    delete worker;
}

void Logger::swap()
{
    bufferB.swap(&bufferA);
    bufferA.reset();        // swap只实现了A复制给B
    // 唤醒B的read线程
    cv_worker.notify_one();
}

std::string Logger::getTime()
{
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << "[" << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S") << "]";
    return ss.str();
}

void Logger::timer()
{
    while (!exit)
    {
        sleep(timeout);
        swap();
    }
}

void Logger::launch()
{
    exit = false;
    timerThread = std::thread(&Logger::timer, this);
    timerThread.detach();
    worker->launch();       // 启动worker
}

void Logger::shutdown()
{
    exit = true;
    // 将剩余日志写入后关闭
    swap();
    worker->shutdown();
}

void Logger::log(const char * message, int level)
{
    // 组织日志消息
    std::string levelStr;
    switch (level)
    {
    case INFO:
        levelStr = "[INFO]";
        break;
    case WARNING:
        levelStr = "[WARNING]";
        break;
    case ERROR:
        levelStr = "[ERROR]";
        break;
    case FATAL:
        levelStr = "[FATAL]";
    default:
        levelStr = "[INFO]";
        break;
    }

    std::stringstream newSS;
    newSS << levelStr << getTime() << " " << message << "\n";
    std::string newMessage = newSS.str();

    if (bufferA.append(newMessage.c_str(), newMessage.size()) == -1) {
        // 交换缓冲区
        swap();
        // 重新log
        bufferA.append(newMessage.c_str(), newMessage.size());
    }
}