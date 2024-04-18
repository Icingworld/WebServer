#ifndef _WEBSERVER_H_
#define _WEBSERVER_H_

#include "../poller/poller.h"
#include "../threadpool/threadpool.h"
#include "../log/logger.h"

class WebServer
{
private:
    ThreadPool threadPool;
    Poller poller;
    Logger logger;

public:
    WebServer(int threadPoolSize, const char * logFilePath);
    ~WebServer();

    void launch();                          // 启动webserver
    void shutdown();                        // 关闭webserver

    void setPort(int newPort);              // 设置监听端口
    void setListenMax(int newListenMax);    // 设置listen排队上限
    void setEventsMax(int newEventsMax);    // 设置epoll_events响应上限
};

#endif