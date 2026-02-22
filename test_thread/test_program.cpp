#include "thread_manager.h"
#include "thread_manager_pthread.h"
#include <iostream>
#include <pthread.h>
#include <unistd.h>

// 子线程函数（C++标准库版本）
void* workerThreadFunc(void* arg) {
    std::string threadName = *static_cast<std::string*>(arg);
    pthread_t threadId = pthread_self();
    
    // 注册线程到ThreadManager
    ThreadManager::getInstance()->registerThread(threadName, threadId);
    
    std::cout << "Worker thread (std::mutex) started: " << threadName << " (ID: " << threadId << ")" << std::endl;
    
    // 调用Sleep函数进入睡眠状态（不需要参数）
    Sleep();
    
    // 再次调用Sleep函数，测试多次睡眠和唤醒
    Sleep();
    
    // 注销线程
    ThreadManager::getInstance()->unregisterThread(threadId);
    
    std::cout << "Worker thread (std::mutex) exited: " << threadName << std::endl;
    return NULL;
}

// 子线程函数（pthread版本）
void* workerThreadFuncPthread(void* arg) {
    std::string threadName = *static_cast<std::string*>(arg);
    pthread_t threadId = pthread_self();
    
    // 注册线程到ThreadManagerPthread
    ThreadManagerPthread::getInstance()->registerThread(threadName, threadId);
    
    std::cout << "Worker thread (pthread) started: " << threadName << " (ID: " << threadId << ")" << std::endl;
    
    // 调用SleepPthread函数进入睡眠状态
    SleepPthread();
    
    // 再次调用SleepPthread函数，测试多次睡眠和唤醒
    SleepPthread();
    
    // 注销线程
    ThreadManagerPthread::getInstance()->unregisterThread(threadId);
    
    std::cout << "Worker thread (pthread) exited: " << threadName << std::endl;
    return NULL;
}

int main() {
    std::cout << "Main thread started" << std::endl;
    
    // 测试C++标准库版本
    std::cout << "\n=== Testing C++ Standard Library Version (std::mutex) ===" << std::endl;
    
    // 创建4个子线程（C++标准库版本）
    pthread_t worker1, worker2, worker3, worker4;
    std::string name1 = "Worker1";
    std::string name2 = "Worker2";
    std::string name3 = "Worker3";
    std::string name4 = "Worker4";
    
    // 启动子线程
    pthread_create(&worker1, NULL, workerThreadFunc, &name1);
    pthread_create(&worker2, NULL, workerThreadFunc, &name2);
    pthread_create(&worker3, NULL, workerThreadFunc, &name3);
    pthread_create(&worker4, NULL, workerThreadFunc, &name4);
    
    // 等待子线程注册并进入睡眠状态
    sleep(2);
    
    // 测试1：使用线程名唤醒子线程
    std::cout << "\n=== Test 1: Waking up threads by name ===" << std::endl;
    Wakeup(name1);
    sleep(1);
    Wakeup(name2);
    sleep(1);
    
    // 等待子线程再次进入睡眠状态
    sleep(2);
    
    // 测试2：使用线程ID唤醒子线程
    std::cout << "\n=== Test 2: Waking up threads by ID ===" << std::endl;
    Wakeup(worker3);
    sleep(1);
    Wakeup(worker4);
    sleep(1);
    
    // 等待子线程再次进入睡眠状态
    sleep(2);
    
    // 测试3：唤醒所有子线程，让它们退出
    std::cout << "\n=== Test 3: Waking up all threads to exit ===" << std::endl;
    Wakeup(name1);
    Wakeup(name2);
    Wakeup(name3);
    Wakeup(name4);
    
    // 等待所有子线程退出
    pthread_join(worker1, NULL);
    pthread_join(worker2, NULL);
    pthread_join(worker3, NULL);
    pthread_join(worker4, NULL);
    
    // 测试pthread版本
    std::cout << "\n=== Testing pthread Version ===" << std::endl;
    
    // 创建4个子线程（pthread版本）
    pthread_t workerP1, workerP2, workerP3, workerP4;
    std::string nameP1 = "WorkerP1";
    std::string nameP2 = "WorkerP2";
    std::string nameP3 = "WorkerP3";
    std::string nameP4 = "WorkerP4";
    
    // 启动子线程
    pthread_create(&workerP1, NULL, workerThreadFuncPthread, &nameP1);
    pthread_create(&workerP2, NULL, workerThreadFuncPthread, &nameP2);
    pthread_create(&workerP3, NULL, workerThreadFuncPthread, &nameP3);
    pthread_create(&workerP4, NULL, workerThreadFuncPthread, &nameP4);
    
    // 等待子线程注册并进入睡眠状态
    sleep(2);
    
    // 测试1：使用线程名唤醒子线程
    std::cout << "\n=== Test 1 (pthread): Waking up threads by name ===" << std::endl;
    WakeupPthread(nameP1);
    sleep(1);
    WakeupPthread(nameP2);
    sleep(1);
    
    // 等待子线程再次进入睡眠状态
    sleep(2);
    
    // 测试2：使用线程ID唤醒子线程
    std::cout << "\n=== Test 2 (pthread): Waking up threads by ID ===" << std::endl;
    WakeupPthread(workerP3);
    sleep(1);
    WakeupPthread(workerP4);
    sleep(1);
    
    // 等待子线程再次进入睡眠状态
    sleep(2);
    
    // 测试3：唤醒所有子线程，让它们退出
    std::cout << "\n=== Test 3 (pthread): Waking up all threads to exit ===" << std::endl;
    WakeupPthread(nameP1);
    WakeupPthread(nameP2);
    WakeupPthread(nameP3);
    WakeupPthread(nameP4);
    
    // 等待所有子线程退出
    pthread_join(workerP1, NULL);
    pthread_join(workerP2, NULL);
    pthread_join(workerP3, NULL);
    pthread_join(workerP4, NULL);
    
    std::cout << "\nMain thread exited" << std::endl;
    return 0;
}