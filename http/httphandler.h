#ifndef _HTTPHANDLER_H_
#define _HTTPHANDLER_H_

#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>

#define RESPONSE_200 std::string("HTTP/1.1 200 OK\r\n")                  // 正常
#define RESPONSE_400 std::string("HTTP/1.1 400 Bad Request\r\n")         // 请求方法错误
#define RESPONSE_404 std::string("HTTP/1.1 404 Not Found\r\n")           // 找不到资源

#define HTML        std::string("Content-Type: text/html\r\n")
#define JS          std::string("Content-Type: application/javascript\r\n")
#define CSS         std::string("Content-Type: text/css\r\n")
#define JPG         std::string("Content-Type: image/jpeg\r\n")
#define PNG         std::string("Content-Type: image/png\r\n")
#define PLAIN       std::string("Content-Type: text/plain\r\n")

class HttpHandler
{
private:
    std::string urlBase;            // 服务器文件系统根目录
    // Logger * logger;

    std::string decodeUrl(const std::string & url);
    std::string extractFileName(const std::string & url);
    std::string readTextFile(const std::string & url);
    std::string readBinaryFile(const std::string & url);

public:
    HttpHandler();
    HttpHandler(std::string urlBase);
    ~HttpHandler();

    int handle(const char * recv, std::string & ret, std::string & logMessage);      // 处理接收请求
};

#endif