#include "thread.h"
#include <unistd.h>

// Thread基类实现
Thread::Thread(const std::string& name) : name(name), running(false) {
    int ret;
    
    // 初始化互斥锁
    ret = pthread_mutex_init(&mutex, NULL);
    if (ret != 0) {
        std::cerr << "Error: pthread_mutex_init failed for " << name << ": " << ret << std::endl;
        // 注意：在构造函数中如果互斥锁初始化失败，可能需要更复杂的错误处理
    }
    
    // 初始化条件变量
    ret = pthread_cond_init(&cond, NULL);
    if (ret != 0) {
        std::cerr << "Error: pthread_cond_init failed for " << name << ": " << ret << std::endl;
        // 注意：在构造函数中如果条件变量初始化失败，可能需要更复杂的错误处理
    }
}

Thread::~Thread() {
    int ret;
    
    // 销毁条件变量
    ret = pthread_cond_destroy(&cond);
    if (ret != 0) {
        std::cerr << "Error: pthread_cond_destroy failed for " << name << ": " << ret << std::endl;
    }
    
    // 销毁互斥锁
    ret = pthread_mutex_destroy(&mutex);
    if (ret != 0) {
        std::cerr << "Error: pthread_mutex_destroy failed for " << name << ": " << ret << std::endl;
    }
}

bool Thread::start() {
    int ret;
    
    if (running) {
        std::cerr << "Error: Thread " << name << " is already running" << std::endl;
        return false;
    }
    
    running = true;
    ret = pthread_create(&tid, NULL, threadFunc, this);
    if (ret != 0) {
        std::cerr << "Error: pthread_create failed for " << name << ": " << ret << std::endl;
        running = false;
        return false;
    }
    
    std::cout << "Thread " << name << " started successfully" << std::endl;
    return true;
}

void* Thread::threadFunc(void* arg) {
    Thread* thread = static_cast<Thread*>(arg);
    thread->run();
    return NULL;
}

// WorkerThread实现
WorkerThread::WorkerThread(const std::string& name) : Thread(name), sleeping(false) {
}

WorkerThread::~WorkerThread() {
}

void WorkerThread::Sleep() {
    int ret;
    
    // 加锁
    ret = pthread_mutex_lock(&mutex);
    if (ret != 0) {
        std::cerr << "Error: pthread_mutex_lock failed for " << name << ": " << ret << std::endl;
        return;
    }
    
    sleeping = true;
    std::cout << name << " is going to sleep..." << std::endl;
    
    while (sleeping) {
        ret = pthread_cond_wait(&cond, &mutex);
        if (ret != 0) {
            if (ret == EINTR) {
                // 被信号中断，继续等待
                std::cout << name << " pthread_cond_wait interrupted by signal, continuing..." << std::endl;
                continue;
            } else {
                // 其他错误，需要处理
                std::cerr << "Error: pthread_cond_wait failed for " << name << ": " << ret << std::endl;
                sleeping = false;  // 设置为false，确保状态一致
                break;
            }
        }
    }
    
    std::cout << name << " is woken up!" << std::endl;
    
    // 解锁
    ret = pthread_mutex_unlock(&mutex);
    if (ret != 0) {
        std::cerr << "Error: pthread_mutex_unlock failed for " << name << ": " << ret << std::endl;
    }
}

void WorkerThread::Wakeup() {
    int ret;
    
    // 加锁
    ret = pthread_mutex_lock(&mutex);
    if (ret != 0) {
        std::cerr << "Error: pthread_mutex_lock failed for " << name << ": " << ret << std::endl;
        return;
    }
    
    if (sleeping) {
        sleeping = false;
        ret = pthread_cond_signal(&cond);
        if (ret != 0) {
            std::cerr << "Error: pthread_cond_signal failed for " << name << ": " << ret << std::endl;
            // 即使信号发送失败，也将sleeping设置为false，确保状态一致
        } else {
            std::cout << name << " is being woken up by main thread" << std::endl;
        }
    }
    
    // 解锁
    ret = pthread_mutex_unlock(&mutex);
    if (ret != 0) {
        std::cerr << "Error: pthread_mutex_unlock failed for " << name << ": " << ret << std::endl;
    }
}

void WorkerThread::run() {
    std::cout << name << " started" << std::endl;
    while (running) {
        Sleep();
    }
    std::cout << name << " exited" << std::endl;
}

// MainThread实现
MainThread::MainThread(const std::string& name) : Thread(name) {
}

MainThread::~MainThread() {
}

void MainThread::run() {
    std::cout << name << " started" << std::endl;
    // 主线程的run方法可以留空，因为唤醒操作会通过外部调用WakeupWorker方法执行
    while (running) {
        sleep(1); // 防止主线程退出
    }
    std::cout << name << " exited" << std::endl;
}

void MainThread::WakeupWorker(WorkerThread* worker) {
    if (worker) {
        worker->Wakeup();
    } else {
        std::cerr << "Error: NULL worker thread pointer" << std::endl;
    }
}