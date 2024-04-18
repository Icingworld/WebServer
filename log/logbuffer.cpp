#include "logbuffer.h"

LogBuffer::LogBuffer()
    : _size(DEFAULT_BUFFER_SIZE)
    , _data(new char[_size])
    , _cur(_data)
    , empty(true)
    , len(0)
{

}

LogBuffer::LogBuffer(LogBuffer * otherBuffer)
    : _size(otherBuffer->size())
    , _data(new char[_size])
    , _cur(_data)
    , empty(otherBuffer->isEmpty())
    , len(otherBuffer->length())
{
    std::unique_lock<std::mutex> lock1(bufferMtx);
    std::unique_lock<std::mutex> lock2(otherBuffer->getMutex());
    memcpy(_data, otherBuffer->data(), otherBuffer->length());
    _cur = _data + len;
}

LogBuffer::~LogBuffer()
{
    delete[] _data;
}

size_t LogBuffer::size() const
{
    return _size;
}

size_t LogBuffer::avail() const
{
    // const char * end = _data + _size;
    // return static_cast<size_t>(end - _cur);
    return _size - len;
}

char * LogBuffer::cur() const
{
    return _cur;
}

size_t LogBuffer::length() const
{
    return len;
}

void LogBuffer::setSize(size_t newSize)
{
    _size = newSize;
}

bool LogBuffer::isEmpty() const
{
    return empty;
}

void LogBuffer::reset()
{
    std::unique_lock<std::mutex> lock(bufferMtx);
    memset(_data, 0, _size);
    _cur = _data;
    empty = true;
    len = 0;
}

/**
 * @return 0: 成功 -1: 失败
**/
int LogBuffer::append(const char * message, size_t strlen)
{
    std::unique_lock<std::mutex> lock(bufferMtx);
    // 判断是否超出剩余空间大小
    if (strlen > avail()) {
        _cur = _data;
        return -1;
    }
    // 插入数据
    memcpy(_cur, message, strlen);
    // 移动指针
    _cur = _cur + strlen;
    len += strlen;
    if (empty) {
        empty = false;
    }
    return 0;
}

void LogBuffer::swap(LogBuffer * otherBuffer) {
    std::unique_lock<std::mutex> lock1(bufferMtx);
    std::unique_lock<std::mutex> lock2(otherBuffer->getMutex());
    _size = otherBuffer->size();
    memcpy(_data, otherBuffer->data(), otherBuffer->length());
    empty = otherBuffer->isEmpty();
    len = otherBuffer->length();
    _cur = _data + len;
}

char * LogBuffer::data() const
{
    return _data;
}

std::mutex & LogBuffer::getMutex()
{
    return bufferMtx;
}

LogBuffer & LogBuffer::operator=(LogBuffer & otherBuffer)
{
    if (this != &otherBuffer) {
        std::unique_lock<std::mutex> lock1(bufferMtx);
        std::unique_lock<std::mutex> lock2(otherBuffer.getMutex());
        _size = otherBuffer.size();
        memcpy(_data, otherBuffer.data(), otherBuffer.length());
        empty = otherBuffer.isEmpty();
        len = otherBuffer.length();
        _cur = _data + len;
    }
    return *this;
}