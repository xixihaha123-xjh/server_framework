#ifndef __UTILITY__H__
#define __UTILITY__H__

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <stdint.h>


namespace utility {

// 获取线程id
pid_t GetThreadId();

// 获取协程id
uint32_t GetFiberId();

}



#endif