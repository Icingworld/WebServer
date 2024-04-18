#include "poller.h"

Poller::Poller(ThreadPool * threadPool, Logger * logger)
    : port(DEFAULT_PORT)
    , listenMax(DEFAULT_LISTEN)
    , eventsMax(DEFAULT_EVENTS)
    , exit(false)
    , threadPool(threadPool)
    , logger(logger)
{

}

Poller::~Poller()
{

}

void Poller::setPort(int newPort)
{
    port = newPort;
}

void Poller::setListenMax(int newQueueMax)
{
    listenMax = newQueueMax;
}

void Poller::setEventsMax(int newMaxEvents)
{
    eventsMax = newMaxEvents;
}

void Poller::mainEvent()
{
    // 创建监听socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        logger->log("Failed to create server socket.", Logger::FATAL);
        return;
    }

    // 绑定地址和端口
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        logger->log("Failed to bind port to server socket.", Logger::FATAL);
        close(server_fd);
        return;
    }

    // 开始监听socket
    if (listen(server_fd, listenMax) == -1) {
        logger->log("Failed to listen server socket.", Logger::FATAL);
        close(server_fd);
        return;
    }

    // 创建epoll对象
    int epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd == -1) {
        logger->log("Failed to create epoll.", Logger::FATAL);
        close(server_fd);
        return;
    }

    // 将服务器套接字添加到epoll中
    struct epoll_event event;
    event.data.fd = server_fd;
    event.events = EPOLLIN;   // 监听可读事件 | 水平触发
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1) {
        // printf("failed to add server socket to epoll: %s\n", strerror(errno));
        logger->log("Failed to add server socket to epoll.", Logger::FATAL);
        close(epoll_fd);
        close(server_fd);
        return;
    }

    struct epoll_event events[eventsMax];
    while (!exit)
    {
        // 等待事件到来
        int eventNum = epoll_wait(epoll_fd, events, eventsMax, DEFAULT_TIMEOUT);
        if (eventNum == -1) {
            // printf("error when waiting for epoll: %s\n", strerror(errno));
            logger->log("Failed to wait for epoll.", Logger::FATAL);
            close(epoll_fd);
            close(server_fd);
            return;
        }

        // 处理事件
        for (int i=0;i<eventNum;i++) {
            if (events[i].data.fd == server_fd) {
                // 有新的连接请求
                struct sockaddr_in client_addr;
                socklen_t client_addr_len = sizeof(client_addr);
                int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
                if (client_fd == -1) {
                    // printf("error when accept sockets: %s\n", strerror(errno));
                    logger->log("Failed to accept client socket.", Logger::FATAL);
                    close(client_fd);
                    continue;
                }
                // 添加client_fd到task队列
                threadPool->appendTask(client_fd);
            }
        }
    }
    // 退出线程
    close(server_fd);
    close(epoll_fd);
}

void Poller::launch()
{
    // 开始监听
    listeningThread = std::thread(&Poller::mainEvent, this);
    listeningThread.detach();
}

void Poller::shutdown()
{
    exit = true;        // 等待wait超时再次判断即可
}