#define DLL_EXPORTS
#include "thread_manager_jp.h"
#include <mutex>
#include <condition_variable>
#include <thread>
#include <iostream>
#include <memory>
#include <atomic>

// 静的インスタンス初期化
ThreadManager* ThreadManager::instance = new ThreadManager();

// コンストラクタ
ThreadManager::ThreadManager() {
    // std::mutexは自動的に初期化されるため、手動操作は不要
}

// デストラクタ
ThreadManager::~ThreadManager() {
    // std::mutexとstd::condition_variableは自動的に破棄されるため、手動クリーンアップは不要
}

// シングルトンインスタンスを取得
ThreadManager* ThreadManager::getInstance() {
    return instance;
}

// スレッドを登録
void ThreadManager::registerThread(const std::string& threadName, std::thread::id threadId) {
    // std::lock_guardを使用してロックのライフサイクルを自動的に管理
    std::lock_guard<std::mutex> lock(mapMutex);
    
    // スレッドが既に存在するかどうかを確認（スレッドIDによる）
    if (threadMap.find(threadId) != threadMap.end()) {
        std::cerr << "Error: Thread already registered" << std::endl;
        return;
    }
    
    // スレッド名が既に存在するかどうかを確認
    if (threadNameToId.find(threadName) != threadNameToId.end()) {
        std::cerr << "Error: Thread name already exists: " << threadName << std::endl;
        return;
    }
    
    // スレッド情報構造体を作成して初期化
    ThreadInfo info;
    info.name = threadName;
    info.mutex = std::make_shared<std::mutex>();
    info.cond = std::make_shared<std::condition_variable>();
    
    // マップに追加
    threadMap.emplace(threadId, info);
    threadNameToId[threadName] = threadId;
    
    std::cout << "Thread registered: " << threadName << std::endl;
    // std::lock_guardは自動的にロックを解除
}

// スレッドの登録を解除
void ThreadManager::unregisterThread(std::thread::id threadId) {
    // std::lock_guardを使用してロックのライフサイクルを自動的に管理
    std::lock_guard<std::mutex> lock(mapMutex);
    
    auto it = threadMap.find(threadId);
    if (it != threadMap.end()) {
        std::string threadName = it->second.name;
        
        // マップから削除
        threadNameToId.erase(threadName);
        threadMap.erase(it);
        
        std::cout << "Thread unregistered: " << threadName << std::endl;
    } else {
        std::cerr << "Error: Thread not found for unregistration" << std::endl;
    }
    // std::lock_guardは自動的にロックを解除
}

// Sleep関数の実装、パラメータは不要
void ThreadManager::Sleep() {
    std::thread::id currentThreadId = std::this_thread::get_id();
    std::string threadName;
    std::shared_ptr<std::mutex> mutex;
    std::shared_ptr<std::condition_variable> cond;
    
    // マップを保護するためにロック、情報を取得してスリープ状態を設定
    {
        std::lock_guard<std::mutex> mapLock(mapMutex);
        
        // スレッドが登録されているかどうかを確認
        auto it = threadMap.find(currentThreadId);
        if (it == threadMap.end()) {
            std::cerr << "Error: Thread not registered!" << std::endl;
            return;
        }
        
        threadName = it->second.name;
        mutex = it->second.mutex;
        cond = it->second.cond;
        it->second.sleeping = true;
    } // マップのロックを解除（std::lock_guardは自動的に解除）
    
    // スレッドミューテックスをロック
    std::unique_lock<std::mutex> threadLock(*mutex);
    
    std::cout << threadName << " is going to sleep..." << std::endl;
    
    // 条件変数を待機、ラムダ式を述語として使用
    // 注意：たとえwait()の前にnotify_one()が呼び出されても、述語がsleeping状態をチェックします
    // sleepingが既にfalseの場合、wait()はすぐに戻り、通知が失われることはありません
    cond->wait(threadLock, [this, currentThreadId]() {
        std::lock_guard<std::mutex> mapLock(mapMutex);
        auto it = threadMap.find(currentThreadId);
        if (it == threadMap.end()) {
            return true; // スレッドが登録解除されたため、待機を終了
        }
        return !it->second.sleeping;
    });
    
    std::cout << threadName << " is woken up!" << std::endl;
    // std::unique_lockは自動的にロックを解除
}

// スレッド名によってスレッドを wake up
void ThreadManager::Wakeup(const std::string& threadName) {
    // std::lock_guardを使用してロックのライフサイクルを自動的に管理
    std::lock_guard<std::mutex> lock(mapMutex);
    
    // スレッドIDを検索
    auto nameIt = threadNameToId.find(threadName);
    if (nameIt == threadNameToId.end()) {
        std::cerr << "Error: Thread not found: " << threadName << std::endl;
        return;
    }
    
    std::thread::id threadId = nameIt->second;
    auto it = threadMap.find(threadId);
    if (it == threadMap.end()) {
        std::cerr << "Error: Thread not found" << std::endl;
        return;
    }
    
    // スレッドがスリープ中かどうかを確認
    if (it->second.sleeping) {
        it->second.sleeping = false;
        it->second.cond->notify_one(); // 待機中のスレッドに通知
        std::cout << "Waking up thread: " << threadName << std::endl;
    }
    // std::lock_guardは自動的にロックを解除
}

// スレッドIDによってスレッドを wake up
void ThreadManager::Wakeup(std::thread::id threadId) {
    // std::lock_guardを使用してロックのライフサイクルを自動的に管理
    std::lock_guard<std::mutex> lock(mapMutex);
    
    // スレッドが登録されているかどうかを確認
    auto it = threadMap.find(threadId);
    if (it == threadMap.end()) {
        std::cerr << "Error: Thread not found" << std::endl;
        return;
    }
    
    std::string threadName = it->second.name;
    
    // スレッドがスリープ中かどうかを確認
    if (it->second.sleeping) {
        it->second.sleeping = false;
        it->second.cond->notify_one(); // 待機中のスレッドに通知
        std::cout << "Waking up thread: " << threadName << std::endl;
    }
    // std::lock_guardは自動的にロックを解除
}

// グローバルSleep関数
void Sleep() {
    ThreadManager::getInstance()->Sleep();
}

// グローバルWakeup関数（スレッド名）
void Wakeup(const std::string& threadName) {
    ThreadManager::getInstance()->Wakeup(threadName);
}

// グローバルWakeup関数（スレッドID）
void Wakeup(std::thread::id threadId) {
    ThreadManager::getInstance()->Wakeup(threadId);
}
