#include "webserver.h"

WebServer::WebServer(int threadPoolSize, const char * logFilePath)
    : logger(logFilePath)
    , threadPool(threadPoolSize, &logger)
    , poller(&threadPool, &logger)
{

}

WebServer::~WebServer()
{

}

void WebServer::launch()
{
    logger.launch();
    logger.log("WebServer start.", Logger::INFO);
    threadPool.launch();
    poller.launch();
}

void WebServer::shutdown()
{
    poller.shutdown();
    threadPool.shutdown();
    logger.log("WebServer stop.", Logger::INFO);
    logger.shutdown();
}

void WebServer::setPort(int newPort)
{
    poller.setPort(newPort);
}

void WebServer::setListenMax(int newListenMax)
{
    poller.setListenMax(newListenMax);
}

void WebServer::setEventsMax(int newEventsMax)
{
    poller.setEventsMax(newEventsMax);
    threadPool.setEventsMax(newEventsMax);
}