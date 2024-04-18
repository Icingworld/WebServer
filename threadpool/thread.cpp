#include "thread.h"

Thread::Thread(std::queue<int> * task, std::mutex * mtx, std::condition_variable * condition, bool * exit, Logger * logger)
    : task(task)
    , mtx(mtx)
    , condition(condition)
    , exit(exit)
    , eventsMax(DEFAULT_EVENTS)
    , logger(logger)
{

}

Thread::~Thread()
{

}

void Thread::getTask()
{
    // std::unique_lock<std::mutex> lock(*mtx);         // 使用互斥量
    while (!(*exit))
    {
        std::unique_lock<std::mutex> lock(*mtx);        // 使用互斥量
        // 释放锁，等待唤醒
        condition->wait(lock);
        // 通知退出
        if (*exit) {
            // 线程结束
            close(epoll_fd);
            return;
        }

        // 获取fd并开始监听
        if (task->size() == 0) {
            break;
        }
        int client_fd = task->front();
        task->pop();
        // lock.unlock();      // 这里不需要解锁，等待处理结束后wait自动释放锁，这样appendTask时一定会等到有线程空闲时才能拿到锁
                            // 缺点是效率略低，考虑更改为动态线程池，负载高时创建新线程，负载低时清理多余线程

        // fcntl(client_fd,F_SETFL,O_NONBLOCK);  // TODO设置非阻塞

        // 监听client socket
        struct epoll_event event;
        event.data.fd = client_fd;
        event.events = EPOLLIN | EPOLLET;  // 监听可读事件
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1) {
            // printf("failed to add client socket to thread epoll: %s\n", strerror(errno));
            logger->log("Failed to add client socket to epoll.", Logger::INFO);
            close(client_fd);
        }
    }
}

void Thread::handle()
{
    while (!(*exit))
    {
        struct epoll_event events[eventsMax];
        int eventNum = epoll_wait(epoll_fd, events, eventsMax, DEFAULT_TIMEOUT);
        if (eventNum == -1) {
            // printf("error when waiting for thread epoll: %s\n", strerror(errno));
            logger->log("Failed to wait for thread epoll.", Logger::FATAL);
            close(epoll_fd);
            return;
        }

        // 解析数据
        for (int i=0;i<eventNum;i++) {
            int client_fd = events[i].data.fd;  // client_fd
            char buffer[1024];                  // 数据buffer
            ssize_t bytesRecv = recv(client_fd, buffer, sizeof(buffer), 0);
            if (bytesRecv == -1) {
                // 读取错误
                logger->log("Failed to receive data from client.", Logger::ERROR);
                // 关闭文件描述符
                close(client_fd);
                continue;
            } else if (bytesRecv == 0) {
                // 关闭连接
                // 关闭文件描述符
                close(client_fd);
                continue;
            } else {
                // 解析请求
                std::string ret, logMessage;
                int res = http.handle(buffer, ret, logMessage);
                ssize_t bytesSend = send(client_fd, ret.c_str(), ret.size(), 0);
                if (bytesSend == -1) {
                    logger->log("Failed to send reponse to client.", Logger::ERROR);
                    close(client_fd);
                }
                if (logger == nullptr) {
                    printf("null\n");
                }
                logger->log(logMessage.c_str());
                if (res == 1) {
                    // 短连接，关闭socket
                    close(client_fd);
                }
            }
        }
    }
    // 退出线程
}

void Thread::launch()
{
    // 初始化epoll
    epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd == -1) {
        logger->log("Failed to create thread epoll.", Logger::FATAL);
        return;
    }

    // 开启线程
    readingTaskThread = std::thread(&Thread::getTask, this);
    readingTaskThread.detach();
    handlingThread = std::thread(&Thread::handle, this);
    handlingThread.detach();
}

void Thread::setEventsMax(int newEventsMax)
{
    eventsMax = newEventsMax;
}
