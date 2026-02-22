#include "thread_manager.h"
#include <unistd.h>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <lock_guard>
#include <unique_lock>

// 静态实例初始化
ThreadManager* ThreadManager::instance = new ThreadManager();

// 构造函数
ThreadManager::ThreadManager() : mapMutexInitialized(true) {
    // std::mutex会自动初始化，不需要手动操作
}

// 初始化映射表互斥锁（使用std::mutex时简化实现）
void ThreadManager::initMapMutex() {
    // std::mutex会自动初始化，此方法保留以保持兼容性
    mapMutexInitialized = true;
}

// 析构函数
ThreadManager::~ThreadManager() {
    // std::mutex和std::condition_variable会自动销毁，不需要手动清理
    // 此析构函数保留以保持兼容性
}

// 获取单例实例
ThreadManager* ThreadManager::getInstance() {
    return instance;
}

// 辅助方法：通过线程名查找线程ID
pthread_t ThreadManager::findThreadIdByName(const std::string& threadName) {
    for (const auto& pair : threadMap) {
        if (pair.second.name == threadName) {
            return pair.first;
        }
    }
    return 0; // 返回0表示未找到
}

// 注册线程
void ThreadManager::registerThread(const std::string& threadName, pthread_t threadId) {
    // 懒加载初始化互斥锁（简化实现）
    initMapMutex();
    
    // 使用std::lock_guard自动管理锁的生命周期
    std::lock_guard<std::mutex> lock(mapMutex);
    
    // 检查线程是否已存在（通过线程ID）
    if (threadMap.find(threadId) != threadMap.end()) {
        std::cerr << "Error: Thread already registered: ID " << threadId << std::endl;
        return;
    }
    
    // 检查线程名是否已存在
    if (findThreadIdByName(threadName) != 0) {
        std::cerr << "Error: Thread name already exists: " << threadName << std::endl;
        return;
    }
    
    // 创建并初始化线程信息结构体
    ThreadInfo info;
    info.name = threadName;
    info.sleeping = false;
    // std::condition_variable和std::mutex会自动初始化
    
    // 添加到映射表
    threadMap[threadId] = info;
    
    std::cout << "Thread registered: " << threadName << " (ID: " << threadId << ")" << std::endl;
    // std::lock_guard会自动解锁
}

// 注销线程
void ThreadManager::unregisterThread(pthread_t threadId) {
    // 懒加载初始化互斥锁（简化实现）
    initMapMutex();
    
    // 使用std::lock_guard自动管理锁的生命周期
    std::lock_guard<std::mutex> lock(mapMutex);
    
    auto it = threadMap.find(threadId);
    if (it != threadMap.end()) {
        std::string threadName = it->second.name;
        
        // 从映射表中删除
        threadMap.erase(it);
        
        // std::condition_variable和std::mutex会自动销毁
        
        std::cout << "Thread unregistered: " << threadName << " (ID: " << threadId << ")" << std::endl;
    } else {
        std::cerr << "Error: Thread not found for unregistration: ID " << threadId << std::endl;
    }
    // std::lock_guard会自动解锁
}

// Sleep函数实现，不需要参数
void ThreadManager::Sleep() {
    pthread_t currentThreadId = pthread_self();
    
    // 懒加载初始化互斥锁（简化实现）
    initMapMutex();
    
    // 加锁保护映射表
    std::lock_guard<std::mutex> mapLock(mapMutex);
    
    // 检查线程是否已注册
    auto it = threadMap.find(currentThreadId);
    if (it == threadMap.end()) {
        std::cerr << "Error: Thread not registered!" << std::endl;
        return;
    }
    
    std::string threadName = it->second.name;
    std::mutex& mutex = it->second.mutex;
    std::condition_variable& cond = it->second.cond;
    
    // 解锁映射表（std::lock_guard会自动解锁）
    
    // 加锁线程互斥锁
    std::unique_lock<std::mutex> threadLock(mutex);
    
    // 更新睡眠状态
    {
        std::lock_guard<std::mutex> mapLock(mapMutex);
        threadMap[currentThreadId].sleeping = true;
    }
    
    std::cout << threadName << " is going to sleep..." << std::endl;
    
    // 等待条件变量，使用lambda表达式作为谓词
    cond.wait(threadLock, [this, currentThreadId]() {
        std::lock_guard<std::mutex> mapLock(mapMutex);
        return !threadMap[currentThreadId].sleeping;
    });
    
    std::cout << threadName << " is woken up!" << std::endl;
    // std::unique_lock会自动解锁
}

// 根据线程名唤醒线程
void ThreadManager::Wakeup(const std::string& threadName) {
    // 懒加载初始化互斥锁（简化实现）
    initMapMutex();
    
    // 使用std::lock_guard自动管理锁的生命周期
    std::lock_guard<std::mutex> lock(mapMutex);
    
    // 查找线程ID（遍历map）
    pthread_t threadId = findThreadIdByName(threadName);
    if (threadId == 0) {
        std::cerr << "Error: Thread not found: " << threadName << std::endl;
        return;
    }
    
    // 检查线程是否在睡眠
    auto it = threadMap.find(threadId);
    if (it != threadMap.end() && it->second.sleeping) {
        it->second.sleeping = false;
        it->second.cond.notify_one(); // 通知等待的线程
        std::cout << "Waking up thread: " << threadName << " (ID: " << threadId << ")" << std::endl;
    }
    // std::lock_guard会自动解锁
}

// 根据线程ID唤醒线程
void ThreadManager::Wakeup(pthread_t threadId) {
    // 懒加载初始化互斥锁（简化实现）
    initMapMutex();
    
    // 使用std::lock_guard自动管理锁的生命周期
    std::lock_guard<std::mutex> lock(mapMutex);
    
    // 检查线程是否已注册
    auto it = threadMap.find(threadId);
    if (it == threadMap.end()) {
        std::cerr << "Error: Thread not found: ID " << threadId << std::endl;
        return;
    }
    
    std::string threadName = it->second.name;
    
    // 检查线程是否在睡眠
    if (it->second.sleeping) {
        it->second.sleeping = false;
        it->second.cond.notify_one(); // 通知等待的线程
        std::cout << "Waking up thread: " << threadName << " (ID: " << threadId << ")" << std::endl;
    }
    // std::lock_guard会自动解锁
}

// 全局Sleep函数
void Sleep() {
    ThreadManager::getInstance()->Sleep();
}

// 全局Wakeup函数（线程名）
void Wakeup(const std::string& threadName) {
    ThreadManager::getInstance()->Wakeup(threadName);
}

// 全局Wakeup函数（线程ID）
void Wakeup(pthread_t threadId) {
    ThreadManager::getInstance()->Wakeup(threadId);
}