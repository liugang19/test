#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include <string>
#include <map>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <iostream>
#include <cerrno>

#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

// 线程信息结构体，包含所有线程相关信息
struct ThreadInfo {
    std::string name;                // 线程名
    bool sleeping;                   // 睡眠状态
    std::condition_variable cond;    // 条件变量
    std::mutex mutex;                // 互斥锁
};

class DLL_API ThreadManager {
public:
    static ThreadManager* getInstance();
    
    // 用户线程调用的Sleep函数，不需要参数
    void Sleep();
    
    // 用户线程调用的Wakeup函数，可以传入线程名或线程id
    void Wakeup(const std::string& threadName);
    void Wakeup(pthread_t threadId);
    
    // 内部使用的方法，用于注册和注销线程
    void registerThread(const std::string& threadName, pthread_t threadId);
    void unregisterThread(pthread_t threadId);
    
private:
    ThreadManager();
    ~ThreadManager();
    ThreadManager(const ThreadManager&) = delete;
    ThreadManager& operator=(const ThreadManager&) = delete;
    
    // 初始化映射表互斥锁
    void initMapMutex();
    
    static ThreadManager* instance;
    
    // 线程信息映射（主键：线程ID）
    std::map<pthread_t, ThreadInfo> threadMap;
    
    // 线程名到线程ID的映射（用于快速查找）
    std::map<std::string, pthread_t> threadNameToId;
    
    // 保护映射表的互斥锁
    std::mutex mapMutex;
    
    // 互斥锁初始化标志（使用std::mutex时可省略，但保留以保持兼容性）
    bool mapMutexInitialized;
};

// 方便用户使用的全局函数
DLL_API void Sleep();
DLL_API void Wakeup(const std::string& threadName);
DLL_API void Wakeup(pthread_t threadId);

#endif // THREAD_MANAGER_H