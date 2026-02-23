#define DLL_EXPORTS
#include "thread_manager_pthread.h"
#include <unistd.h>

// 静态实例初始化
ThreadManagerPthread* ThreadManagerPthread::instance = new ThreadManagerPthread();

// 构造函数
ThreadManagerPthread::ThreadManagerPthread() : mapMutexInitialized(false) {
    // 互斥锁将在第一次使用时懒加载初始化
}

// 初始化映射表互斥锁
void ThreadManagerPthread::initMapMutex() {
    if (!mapMutexInitialized) {
        int ret = pthread_mutex_init(&mapMutex, NULL);
        if (ret != 0) {
            std::cerr << "Error: pthread_mutex_init failed for mapMutex: " << ret << std::endl;
            return;
        }
        mapMutexInitialized = true;
    }
}

// 析构函数
ThreadManagerPthread::~ThreadManagerPthread() {
    int ret;
    
    // 清理所有线程的条件变量和互斥锁
    for (auto& pair : threadMap) {
        ret = pthread_cond_destroy(&pair.second.cond);
        if (ret != 0) {
            std::cerr << "Error: pthread_cond_destroy failed for thread " << pair.second.name << ": " << ret << std::endl;
        }
        ret = pthread_mutex_destroy(&pair.second.mutex);
        if (ret != 0) {
            std::cerr << "Error: pthread_mutex_destroy failed for thread " << pair.second.name << ": " << ret << std::endl;
        }
    }
    
    // 只有当互斥锁初始化后才销毁它
    if (mapMutexInitialized) {
        ret = pthread_mutex_destroy(&mapMutex);
        if (ret != 0) {
            std::cerr << "Error: pthread_mutex_destroy failed for mapMutex: " << ret << std::endl;
        }
    }
}

// 获取单例实例
ThreadManagerPthread* ThreadManagerPthread::getInstance() {
    return instance;
}

// 注册线程
void ThreadManagerPthread::registerThread(const std::string& threadName, pthread_t threadId) {
    int ret;
    
    // 懒加载初始化互斥锁
    initMapMutex();
    
    // 加锁保护映射表
    ret = pthread_mutex_lock(&mapMutex);
    if (ret != 0) {
        std::cerr << "Error: pthread_mutex_lock failed for mapMutex: " << ret << std::endl;
        return;
    }
    
    // 检查线程是否已存在（通过线程ID）
    if (threadMap.find(threadId) != threadMap.end()) {
        std::cerr << "Error: Thread already registered: ID " << threadId << std::endl;
        pthread_mutex_unlock(&mapMutex);
        return;
    }
    
    // 检查线程名是否已存在
    if (threadNameToId.find(threadName) != threadNameToId.end()) {
        std::cerr << "Error: Thread name already exists: " << threadName << std::endl;
        pthread_mutex_unlock(&mapMutex);
        return;
    }
    
    // 创建并初始化线程信息结构体
    ThreadInfoPthread info(threadName);
    
    // 初始化条件变量
    ret = pthread_cond_init(&info.cond, NULL);
    if (ret != 0) {
        std::cerr << "Error: pthread_cond_init failed for thread " << threadName << ": " << ret << std::endl;
        pthread_mutex_unlock(&mapMutex);
        return;
    }
    
    // 初始化互斥锁
    ret = pthread_mutex_init(&info.mutex, NULL);
    if (ret != 0) {
        std::cerr << "Error: pthread_mutex_init failed for thread " << threadName << ": " << ret << std::endl;
        pthread_cond_destroy(&info.cond); // 清理已初始化的条件变量
        pthread_mutex_unlock(&mapMutex);
        return;
    }
    
    // 添加到映射表
    threadMap[threadId] = info;
    threadNameToId[threadName] = threadId;
    
    std::cout << "Thread registered: " << threadName << " (ID: " << threadId << ")" << std::endl;
    
    // 解锁
    ret = pthread_mutex_unlock(&mapMutex);
    if (ret != 0) {
        std::cerr << "Error: pthread_mutex_unlock failed for mapMutex: " << ret << std::endl;
    }
}

// 注销线程
void ThreadManagerPthread::unregisterThread(pthread_t threadId) {
    int ret;
    
    // 懒加载初始化互斥锁
    initMapMutex();
    
    // 加锁保护映射表
    ret = pthread_mutex_lock(&mapMutex);
    if (ret != 0) {
        std::cerr << "Error: pthread_mutex_lock failed for mapMutex: " << ret << std::endl;
        return;
    }
    
    auto it = threadMap.find(threadId);
    if (it != threadMap.end()) {
        std::string threadName = it->second.name;
        
        // 清理条件变量和互斥锁
        ret = pthread_cond_destroy(&it->second.cond);
        if (ret != 0) {
            std::cerr << "Error: pthread_cond_destroy failed for thread " << threadName << ": " << ret << std::endl;
        }
        
        ret = pthread_mutex_destroy(&it->second.mutex);
        if (ret != 0) {
            std::cerr << "Error: pthread_mutex_destroy failed for thread " << threadName << ": " << ret << std::endl;
        }
        
        // 清理映射表
        threadNameToId.erase(threadName);
        threadMap.erase(it);
        
        std::cout << "Thread unregistered: " << threadName << " (ID: " << threadId << ")" << std::endl;
    } else {
        std::cerr << "Error: Thread not found for unregistration: ID " << threadId << std::endl;
    }
    
    // 解锁
    ret = pthread_mutex_unlock(&mapMutex);
    if (ret != 0) {
        std::cerr << "Error: pthread_mutex_unlock failed for mapMutex: " << ret << std::endl;
    }
}

// Sleep函数实现，不需要参数
void ThreadManagerPthread::Sleep() {
    int ret;
    pthread_t currentThreadId = pthread_self();
    
    // 懒加载初始化互斥锁
    initMapMutex();
    
    // 加锁保护映射表
    ret = pthread_mutex_lock(&mapMutex);
    if (ret != 0) {
        std::cerr << "Error: pthread_mutex_lock failed for mapMutex: " << ret << std::endl;
        return;
    }
    
    // 检查线程是否已注册
    auto it = threadMap.find(currentThreadId);
    if (it == threadMap.end()) {
        std::cerr << "Error: Thread not registered!" << std::endl;
        pthread_mutex_unlock(&mapMutex);
        return;
    }
    
    std::string threadName = it->second.name;
    pthread_mutex_t& mutex = it->second.mutex;
    pthread_cond_t& cond = it->second.cond;
    
    // 解锁映射表
    ret = pthread_mutex_unlock(&mapMutex);
    if (ret != 0) {
        std::cerr << "Error: pthread_mutex_unlock failed for mapMutex: " << ret << std::endl;
        return;
    }
    
    // 加锁线程互斥锁
    ret = pthread_mutex_lock(&mutex);
    if (ret != 0) {
        std::cerr << "Error: pthread_mutex_lock failed for thread " << threadName << ": " << ret << std::endl;
        return;
    }
    
    // 更新睡眠状态
    {
        ret = pthread_mutex_lock(&mapMutex);
        if (ret != 0) {
            std::cerr << "Error: pthread_mutex_lock failed for mapMutex: " << ret << std::endl;
            pthread_mutex_unlock(&mutex);
            return;
        }
        threadMap[currentThreadId].sleeping = true;
        ret = pthread_mutex_unlock(&mapMutex);
        if (ret != 0) {
            std::cerr << "Error: pthread_mutex_unlock failed for mapMutex: " << ret << std::endl;
            pthread_mutex_unlock(&mutex);
            return;
        }
    }
    std::cout << threadName << " is going to sleep..." << std::endl;
    
    // 等待条件变量
    while (true) {
        ret = pthread_mutex_lock(&mapMutex);
        if (ret != 0) {
            std::cerr << "Error: pthread_mutex_lock failed for mapMutex: " << ret << std::endl;
            pthread_mutex_unlock(&mutex);
            return;
        }
        
        bool shouldWait = true;
        auto it = threadMap.find(currentThreadId);
        if (it != threadMap.end()) {
            shouldWait = it->second.sleeping;
        } else {
            shouldWait = false; // 线程已注销，退出等待
        }
        
        ret = pthread_mutex_unlock(&mapMutex);
        if (ret != 0) {
            std::cerr << "Error: pthread_mutex_unlock failed for mapMutex: " << ret << std::endl;
            pthread_mutex_unlock(&mutex);
            return;
        }
        
        if (!shouldWait) {
            break;
        }
        
        ret = pthread_cond_wait(&cond, &mutex);
        if (ret != 0) {
            // 在Linux上，EINTR表示被信号中断
            // 在QNX上，被信号中断会返回EOK，不会进入此分支
            std::cerr << "Error: pthread_cond_wait failed for thread " << threadName << ": " << ret << std::endl;
            {
                ret = pthread_mutex_lock(&mapMutex);
                if (ret != 0) {
                    std::cerr << "Error: pthread_mutex_lock failed for mapMutex: " << ret << std::endl;
                    pthread_mutex_unlock(&mutex);
                    return;
                }
                auto it = threadMap.find(currentThreadId);
                if (it != threadMap.end()) {
                    it->second.sleeping = false;
                }
                ret = pthread_mutex_unlock(&mapMutex);
                if (ret != 0) {
                    std::cerr << "Error: pthread_mutex_unlock failed for mapMutex: " << ret << std::endl;
                    pthread_mutex_unlock(&mutex);
                    return;
                }
            }
            break;
        }
        // 无论在Linux还是QNX上，都会执行到这里
        // 通过while循环再次检查条件，防止虚假唤醒
    }
    
    std::cout << threadName << " is woken up!" << std::endl;
    
    // 解锁线程互斥锁
    ret = pthread_mutex_unlock(&mutex);
    if (ret != 0) {
        std::cerr << "Error: pthread_mutex_unlock failed for thread " << threadName << ": " << ret << std::endl;
    }
}

// 根据线程名唤醒线程
void ThreadManagerPthread::Wakeup(const std::string& threadName) {
    int ret;
    
    // 懒加载初始化互斥锁
    initMapMutex();
    
    // 加锁保护映射表
    ret = pthread_mutex_lock(&mapMutex);
    if (ret != 0) {
        std::cerr << "Error: pthread_mutex_lock failed for mapMutex: " << ret << std::endl;
        return;
    }
    
    // 查找线程ID
    auto nameIt = threadNameToId.find(threadName);
    if (nameIt == threadNameToId.end()) {
        std::cerr << "Error: Thread not found: " << threadName << std::endl;
        pthread_mutex_unlock(&mapMutex);
        return;
    }
    
    pthread_t threadId = nameIt->second;
    auto it = threadMap.find(threadId);
    if (it == threadMap.end()) {
        std::cerr << "Error: Thread not found: ID " << threadId << std::endl;
        pthread_mutex_unlock(&mapMutex);
        return;
    }
    
    // 检查线程是否在睡眠
    if (it->second.sleeping) {
        it->second.sleeping = false;
        ret = pthread_cond_signal(&it->second.cond);
        if (ret != 0) {
            std::cerr << "Error: pthread_cond_signal failed for thread " << threadName << ": " << ret << std::endl;
        } else {
            std::cout << "Waking up thread: " << threadName << " (ID: " << threadId << ")" << std::endl;
        }
    }
    
    // 解锁
    ret = pthread_mutex_unlock(&mapMutex);
    if (ret != 0) {
        std::cerr << "Error: pthread_mutex_unlock failed for mapMutex: " << ret << std::endl;
    }
}

// 根据线程ID唤醒线程
void ThreadManagerPthread::Wakeup(pthread_t threadId) {
    int ret;
    
    // 懒加载初始化互斥锁
    initMapMutex();
    
    // 加锁保护映射表
    ret = pthread_mutex_lock(&mapMutex);
    if (ret != 0) {
        std::cerr << "Error: pthread_mutex_lock failed for mapMutex: " << ret << std::endl;
        return;
    }
    
    // 检查线程是否已注册
    auto it = threadMap.find(threadId);
    if (it == threadMap.end()) {
        std::cerr << "Error: Thread not found: ID " << threadId << std::endl;
        pthread_mutex_unlock(&mapMutex);
        return;
    }
    
    std::string threadName = it->second.name;
    
    // 检查线程是否在睡眠
    if (it->second.sleeping) {
        it->second.sleeping = false;
        ret = pthread_cond_signal(&it->second.cond);
        if (ret != 0) {
            std::cerr << "Error: pthread_cond_signal failed for thread " << threadName << ": " << ret << std::endl;
        } else {
            std::cout << "Waking up thread: " << threadName << " (ID: " << threadId << ")" << std::endl;
        }
    }
    
    // 解锁
    ret = pthread_mutex_unlock(&mapMutex);
    if (ret != 0) {
        std::cerr << "Error: pthread_mutex_unlock failed for mapMutex: " << ret << std::endl;
    }
}

// 全局Sleep函数
void SleepPthread() {
    ThreadManagerPthread::getInstance()->Sleep();
}

// 全局Wakeup函数（线程名）
void WakeupPthread(const std::string& threadName) {
    ThreadManagerPthread::getInstance()->Wakeup(threadName);
}

// 全局Wakeup函数（线程ID）
void WakeupPthread(pthread_t threadId) {
    ThreadManagerPthread::getInstance()->Wakeup(threadId);
}
