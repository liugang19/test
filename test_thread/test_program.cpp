#include "thread_manager.h"
#include <iostream>
#include <thread>
#include <chrono>

// 子线程函数（C++标准库版本）
void workerThreadFunc(const std::string& threadName) {
    std::thread::id threadId = std::this_thread::get_id();
    
    // 注册线程到ThreadManager
    ThreadManager::getInstance()->registerThread(threadName, threadId);
    
    std::cout << "Worker thread started: " << threadName << std::endl;
    
    // 调用Sleep函数进入睡眠状态（不需要参数）
    Sleep();
    
    // 再次调用Sleep函数，测试多次睡眠和唤醒
    Sleep();
    
    // 注销线程
    ThreadManager::getInstance()->unregisterThread(threadId);
    
    std::cout << "Worker thread exited: " << threadName << std::endl;
}

int main() {
    std::cout << "Main thread started" << std::endl;
    
    // 测试C++标准库版本
    std::cout << "\n=== Testing C++ Standard Library Version ===" << std::endl;
    
    // 创建4个子线程
    std::thread worker1(workerThreadFunc, "Worker1");
    std::thread worker2(workerThreadFunc, "Worker2");
    std::thread worker3(workerThreadFunc, "Worker3");
    std::thread worker4(workerThreadFunc, "Worker4");
    
    // 等待子线程注册并进入睡眠状态
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // 测试1：使用线程名唤醒子线程
    std::cout << "\n=== Test 1: Waking up threads by name ===" << std::endl;
    Wakeup("Worker1");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    Wakeup("Worker2");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // 等待子线程再次进入睡眠状态
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // 测试2：使用线程名唤醒剩余子线程
    std::cout << "\n=== Test 2: Waking up remaining threads ===" << std::endl;
    Wakeup("Worker3");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    Wakeup("Worker4");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // 等待子线程再次进入睡眠状态
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // 测试3：唤醒所有子线程，让它们退出
    std::cout << "\n=== Test 3: Waking up all threads to exit ===" << std::endl;
    Wakeup("Worker1");
    Wakeup("Worker2");
    Wakeup("Worker3");
    Wakeup("Worker4");
    
    // 等待所有子线程退出
    worker1.join();
    worker2.join();
    worker3.join();
    worker4.join();
    
    std::cout << "\nMain thread exited" << std::endl;
    return 0;
}