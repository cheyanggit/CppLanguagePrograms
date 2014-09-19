﻿// ***********************************************************************
// Filename         : Semaphore.h
// Author           : LIZHENG
// Created          : 2014-06-08
// Description      : 同步阻塞队列，可工作于多线程环境下，可用于线程之间数据存取
//
// Last Modified By : LIZHENG
// Last Modified On : 2014-08-25
//
// Copyright (c) lizhenghn@gmail.com. All rights reserved.
// ***********************************************************************
#ifndef ZL_SEMAPHORE_H
#define ZL_SEMAPHORE_H
#include "OsDefine.h"
#include "NonCopy.h"
#include <exception>
#ifdef OS_WINDOWS
#include <Windows.h>
#elif defined(OS_LINUX)
#define OS_LINUX
typedef unsigned long DWORD;
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#endif

namespace ZL
{
    class Semaphore : public ZL::NonCopy
    {
    public:
        explicit Semaphore(int initial = 0)
        {
#ifdef OS_WINDOWS
            sem_ = ::CreateSemaphore(NULL, initial, 0x7fffffff, NULL);
#elif defined(OS_LINUX)
            if(sem_init(&sem_, 0, 0) != 0)
            {
                throw std::exception();
            }
#endif
        }
        ~Semaphore()
        {
#ifdef OS_WINDOWS
            ::CloseHandle(sem_);
#elif defined(OS_LINUX)
            sem_destroy(&sem_);
#endif
        }

    public:
        // 以原子操作的方式将信号量减1，如果信号量的值为0，则阻塞，直到该值不为0.
        bool Wait(DWORD duration = INFINITE)
        {
#ifdef OS_WINDOWS
            if(::WaitForSingleObject(&sem_, duration) == WAIT_OBJECT_0)
                return true;
            return false;
#elif defined(OS_LINUX)
            return sem_wait(&sem_) == 0;
#endif
        }
        // Wait的非阻塞版本
        bool TryWait()
        {
#ifdef OS_WINDOWS
            return ::WaitForSingleObject(sem_, 0) == WAIT_OBJECT_0;
#elif defined(OS_LINUX)
            return sem_trywait(&sem_) == 0;
#endif
        }

        // 以原子操作的方式将信号量的值加rc
        bool Post(long rc = 1)
        {
#ifdef OS_WINDOWS
            if(::ReleaseSemaphore(sem_, rc, NULL))
                return true;
            return false;
#elif defined(OS_LINUX)
            return sem_post(&sem_) == 0;
#endif
        }

    private:
#ifdef OS_WINDOWS
        HANDLE sem_;
#elif defined(OS_LINUX)
        sem_t sem_;
#endif
    };

} /* namespace ZL */

#endif  /* ZL_SEMAPHORE_H */