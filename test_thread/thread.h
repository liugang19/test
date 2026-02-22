#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>
#include <string>
#include <iostream>
#include <cerrno>

class Thread {
public:
    Thread(const std::string& name);
    virtual ~Thread();
    
    bool start();
    virtual void run() = 0;
    
protected:
    std::string name;
    pthread_t tid;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    bool running;
    
private:
    static void* threadFunc(void* arg);
};

class WorkerThread : public Thread {
public:
    WorkerThread(const std::string& name);
    ~WorkerThread();
    
    void Sleep();
    void Wakeup();
    void run() override;
    
private:
    bool sleeping;
};

class MainThread : public Thread {
public:
    MainThread(const std::string& name);
    ~MainThread();
    
    void run() override;
    void WakeupWorker(WorkerThread* worker);
};

#endif // THREAD_H