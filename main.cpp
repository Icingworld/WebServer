#include <csignal>

#include "webserver/webserver.h"

WebServer * webserver = nullptr;
std::mutex mtx;
std::condition_variable cv_main;

void mainExit(int signum)
{
    webserver->shutdown();
    delete webserver;
    cv_main.notify_one();
    printf("webserver shutdown with code: %d\n", signum);
}

int main()
{
    signal(SIGINT, mainExit);
    signal(SIGTERM, mainExit);

    webserver = new WebServer(4, "/var/log/webserver/log.log");
    // 默认配置见 global_define.h
    webserver->setPort(1880);               // 设置端口，默认80
    webserver->setListenMax(512);           // 设置socket最多监听数，默认256
    webserver->setEventsMax(512);           // 设置epoll最多响应事件数，默认256
    webserver->launch();

    // 休眠主线程
    std::unique_lock<std::mutex> lock(mtx);
    cv_main.wait(lock);

    return 0;
}
