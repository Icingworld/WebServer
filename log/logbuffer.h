#ifndef _LOGBUFFER_H_
#define _LOGBUFFER_H_

#include <cstddef>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <mutex>

#include "../global_define.h"

/**
 * @brief 缓冲区
*/
class LogBuffer
{
private:
    size_t _size;                                           // 缓冲区大小
    char * _data;                                           // 缓冲区数组
    char * _cur;                                            // 当前指针
    bool empty;                                             // 缓冲区是否为空
    size_t len;                                             // 缓冲区数据长度
    std::mutex bufferMtx;                                   // 互斥量

    size_t avail() const;                                   // 剩余缓冲区大小

public:
    LogBuffer();
    LogBuffer(LogBuffer * otherBuffer);
    ~LogBuffer();

    size_t size() const;                                    // 获取缓冲区大小
    void setSize(size_t newSize);                           // 设置缓冲区大小
    char * cur() const;                                     // 获取当前指针位置
    size_t length() const;                                  // 数据长度
    bool isEmpty() const;                                   // 是否空闲
    void reset();                                           // 重置缓冲区
    int append(const char * message, size_t len);           // 插入数据
    char * data() const;                                    // 获取缓冲区
    void swap(LogBuffer * otherBuffer);                     // 交换缓冲区，保留
    std::mutex & getMutex();                                // 获取互斥量
    LogBuffer & operator=(LogBuffer & otherBuffer);
};

#endif