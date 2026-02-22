#include "thread.h"
#include <iostream>
#include <unistd.h>

int main() {
    // 创建主线程和4个子线程
    MainThread mainThread("MainThread");
    WorkerThread worker1("WorkerThread1");
    WorkerThread worker2("WorkerThread2");
    WorkerThread worker3("WorkerThread3");
    WorkerThread worker4("WorkerThread4");
    
    // 启动所有线程
    mainThread.start();
    worker1.start();
    worker2.start();
    worker3.start();
    worker4.start();
    
    // 等待一段时间，让子线程进入睡眠状态
    sleep(2);
    
    // 测试主线程唤醒子线程
    std::cout << "\n=== Test 1: Waking up all worker threads ===" << std::endl;
    mainThread.WakeupWorker(&worker1);
    sleep(1);
    mainThread.WakeupWorker(&worker2);
    sleep(1);
    mainThread.WakeupWorker(&worker3);
    sleep(1);
    mainThread.WakeupWorker(&worker4);
    
    // 等待子线程再次进入睡眠状态
    sleep(2);
    
    // 再次测试唤醒功能
    std::cout << "\n=== Test 2: Waking up worker threads again ===" << std::endl;
    mainThread.WakeupWorker(&worker1);
    sleep(1);
    mainThread.WakeupWorker(&worker3);
    sleep(1);
    
    // 等待用户输入，然后退出程序
    std::cout << "\nPress Enter to exit..." << std::endl;
    std::cin.get();
    
    return 0;
}