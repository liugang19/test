#ifndef THREAD_MANAGER_PTHREAD_H
#define THREAD_MANAGER_PTHREAD_H

#include <string>
#include <map>
#include <pthread.h>
#include <iostream>
#include <cerrno>

#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

// 线程信息结构体，包含所有线程相关信息
struct ThreadInfoPthread {
    std::string name;        // 线程名
    bool sleeping;           // 睡眠状态
    pthread_cond_t cond;     // 条件变量
    pthread_mutex_t mutex;   // 互斥锁
};

class DLL_API ThreadManagerPthread {
public:
    static ThreadManagerPthread* getInstance();
    
    // 用户线程调用的Sleep函数，不需要参数
    void Sleep();
    
    // 用户线程调用的Wakeup函数，可以传入线程名或线程id
    void Wakeup(const std::string& threadName);
    void Wakeup(pthread_t threadId);
    
    // 内部使用的方法，用于注册和注销线程
    void registerThread(const std::string& threadName, pthread_t threadId);
    void unregisterThread(pthread_t threadId);
    
private:
    ThreadManagerPthread();
    ~ThreadManagerPthread();
    ThreadManagerPthread(const ThreadManagerPthread&) = delete;
    ThreadManagerPthread& operator=(const ThreadManagerPthread&) = delete;
    
    // 初始化映射表互斥锁
    void initMapMutex();
    
    static ThreadManagerPthread* instance;
    
    // 线程信息映射（主键：线程ID）
    std::map<pthread_t, ThreadInfoPthread> threadMap;
    
    // 线程名到线程ID的映射（用于快速查找）
    std::map<std::string, pthread_t> threadNameToId;
    
    // 保护映射表的互斥锁
    pthread_mutex_t mapMutex;
    
    // 互斥锁初始化标志
    bool mapMutexInitialized;
};

// 方便用户使用的全局函数
DLL_API void SleepPthread();
DLL_API void WakeupPthread(const std::string& threadName);
DLL_API void WakeupPthread(pthread_t threadId);

#endif // THREAD_MANAGER_PTHREAD_H
