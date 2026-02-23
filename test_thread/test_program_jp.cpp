#include "thread_manager_jp.h"
#include <iostream>
#include <thread>
#include <chrono>

// ワーカースレッド関数（C++標準ライブラリバージョン）
void workerThreadFunc(const std::string& threadName) {
    std::thread::id threadId = std::this_thread::get_id();
    
    // ThreadManagerにスレッドを登録
    ThreadManager::getInstance()->registerThread(threadName, threadId);
    
    std::cout << "Worker thread started: " << threadName << std::endl;
    
    // Sleep関数を呼び出してスリープ状態に入る（パラメータ不要）
    Sleep();
    
    // 複数回のスリープとウェイクアップをテストするために、再度Sleep関数を呼び出す
    Sleep();
    
    // スレッドの登録を解除
    ThreadManager::getInstance()->unregisterThread(threadId);
    
    std::cout << "Worker thread exited: " << threadName << std::endl;
}

int main() {
    std::cout << "Main thread started" << std::endl;
    
    // C++標準ライブラリバージョンのテスト
    std::cout << "\n=== Testing C++ Standard Library Version ===" << std::endl;
    
    // 4つのワーカースレッドを作成
    std::thread worker1(workerThreadFunc, "Worker1");
    std::thread worker2(workerThreadFunc, "Worker2");
    std::thread worker3(workerThreadFunc, "Worker3");
    std::thread worker4(workerThreadFunc, "Worker4");
    
    // 子スレッドが登録されてスリープ状態に入るのを待つ
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // テスト1：スレッド名で子スレッドをウェイクアップ
    std::cout << "\n=== Test 1: Waking up threads by name ===" << std::endl;
    Wakeup("Worker1");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    Wakeup("Worker2");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // 子スレッドが再度スリープ状態に入るのを待つ
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // テスト2：残りの子スレッドをウェイクアップ
    std::cout << "\n=== Test 2: Waking up remaining threads ===" << std::endl;
    Wakeup("Worker3");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    Wakeup("Worker4");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // 子スレッドが再度スリープ状態に入るのを待つ
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // テスト3：すべての子スレッドをウェイクアップして終了させる
    std::cout << "\n=== Test 3: Waking up all threads to exit ===" << std::endl;
    Wakeup("Worker1");
    Wakeup("Worker2");
    Wakeup("Worker3");
    Wakeup("Worker4");
    
    // すべての子スレッドの終了を待つ
    worker1.join();
    worker2.join();
    worker3.join();
    worker4.join();
    
    std::cout << "\nMain thread exited" << std::endl;
    return 0;
}