#ifndef _GLOBAL_DEFINE_H_
#define _GLOBAL_DEFINE_H_

/* EPOLL */
#define DEFAULT_PORT            80      // 默认端口
#define DEFAULT_LISTEN          256     // 默认listen数量
#define DEFAULT_EVENTS          256     // 默认epoll响应事件数
#define DEFAULT_TIMEOUT         3000    // 默认epoll_wait超时

/* LOG */
#define DEFAULT_BUFFER_SIZE     300UL   // 默认缓冲区大小
#define DEFAULT_SWAP_TIME       5       // 默认缓冲区交换时间
#define DEFAULT_QUEUE_MAX       30      // 默认缓冲区最大队列

#endif