#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include <string>
#include <map>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <iostream>

#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

// スレッド情報構造体、すべてのスレッド関連情報を含む
struct ThreadInfo {
    std::string name;                                  // スレッド名
    std::shared_ptr<std::mutex> mutex;                // ミューテックス
    std::shared_ptr<std::condition_variable> cond;    // 条件変数
    bool sleeping{false};                              // スリープ状態
};

class DLL_API ThreadManager {
public:
    static ThreadManager* getInstance();
    
    // ユーザースレッドが呼び出すSleep関数、パラメータは不要
    void Sleep();
    
    // ユーザースレッドが呼び出すWakeup関数、スレッド名またはスレッドIDを渡すことができる
    void Wakeup(const std::string& threadName);
    void Wakeup(std::thread::id threadId);
    
    // 内部使用メソッド、スレッドの登録と登録解除に使用
    void registerThread(const std::string& threadName, std::thread::id threadId);
    void unregisterThread(std::thread::id threadId);
    
private:
    ThreadManager();
    ~ThreadManager();
    ThreadManager(const ThreadManager&) = delete;
    ThreadManager& operator=(const ThreadManager&) = delete;
    
    static ThreadManager* instance;
    
    // スレッド情報マップ（主キー：スレッドID）
    std::map<std::thread::id, ThreadInfo> threadMap;
    
    // スレッド名からスレッドIDへのマップ（高速検索用）
    std::map<std::string, std::thread::id> threadNameToId;
    
    // マップを保護するミューテックス
    std::mutex mapMutex;
};

// ユーザーが使用しやすいようにするグローバル関数
DLL_API void Sleep();
DLL_API void Wakeup(const std::string& threadName);
DLL_API void Wakeup(std::thread::id threadId);

#endif // THREAD_MANAGER_H