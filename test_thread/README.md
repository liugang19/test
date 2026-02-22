# 线程睡眠和唤醒系统

本项目实现了一个线程睡眠和唤醒系统，支持通过线程名或线程ID唤醒线程。项目分为Linux和QNX两个版本，使用pthread库实现线程的无限睡眠和唤醒功能。

## 目录结构

```
test_thread/
├── linux/         # Linux系统下的代码
│   ├── thread_manager.h    # 线程管理器头文件
│   ├── thread_manager.cpp  # 线程管理器实现
│   ├── test_program.cpp    # 测试程序
│   └── Makefile            # Linux编译脚本
├── qnx/           # QNX系统下的代码
│   ├── thread_manager.h    # 线程管理器头文件
│   ├── thread_manager.cpp  # 线程管理器实现
│   ├── test_program.cpp    # 测试程序
│   └── Makefile            # QNX编译脚本
└── README.md      # 本说明文件
```

## 功能特性

1. **线程注册**：将线程注册到线程管理器中，分配唯一的线程名和线程ID
2. **线程睡眠**：线程可以调用`Sleep()`函数进入无限睡眠状态，不需要参数
3. **线程唤醒**：可以通过线程名或线程ID唤醒线程
4. **线程注销**：线程退出前需要注销，释放相关资源
5. **异常处理**：所有pthread函数都有返回值检查，确保系统稳定性
6. **单例模式**：线程管理器采用单例模式，方便全局访问

## Linux系统编译和运行

### 环境要求

- Linux操作系统
- g++编译器
- pthread库

### 编译步骤

1. 进入Linux目录
   ```bash
   cd linux
   ```

2. 执行make命令编译
   ```bash
   make
   ```

3. 编译成功后会生成`thread_test`可执行文件

### 运行步骤

1. 执行编译生成的可执行文件
   ```bash
   ./thread_test
   ```

2. 观察输出结果，测试线程的睡眠和唤醒功能

### 清理编译产物

```bash
make clean
```

## QNX系统编译和运行

### 环境要求

- QNX操作系统
- qcc编译器
- pthread库

### 编译步骤

1. 进入QNX目录
   ```bash
   cd qnx
   ```

2. 执行make命令编译
   ```bash
   make
   ```

3. 编译成功后会生成`thread_test`可执行文件

### 运行步骤

1. 执行编译生成的可执行文件
   ```bash
   ./thread_test
   ```

2. 观察输出结果，测试线程的睡眠和唤醒功能

### 清理编译产物

```bash
make clean
```

## 使用方法

### 1. 注册线程

```cpp
#include "thread_manager.h"

// 在子线程中注册
void* workerThreadFunc(void* arg) {
    std::string threadName = "Worker1";
    pthread_t threadId = pthread_self();
    
    // 注册线程到ThreadManager
    ThreadManager::getInstance()->registerThread(threadName, threadId);
    
    // ... 其他代码 ...
    
    return NULL;
}
```

### 2. 进入睡眠状态

```cpp
// 调用Sleep函数进入睡眠状态（不需要参数）
Sleep();
```

### 3. 唤醒线程

```cpp
// 通过线程名唤醒
Wakeup("Worker1");

// 通过线程ID唤醒
Wakeup(threadId);
```

### 4. 注销线程

```cpp
// 在子线程退出前注销
ThreadManager::getInstance()->unregisterThread(pthread_self());
```

## 运行示例

运行测试程序后，会看到类似以下输出：

```
Main thread started
Thread registered: Worker1 (ID: 12345)
Worker thread started: Worker1 (ID: 12345)
Worker1 is going to sleep...
Thread registered: Worker2 (ID: 67890)
Worker thread started: Worker2 (ID: 67890)
Worker2 is going to sleep...
Thread registered: Worker3 (ID: 54321)
Worker thread started: Worker3 (ID: 54321)
Worker3 is going to sleep...
Thread registered: Worker4 (ID: 09876)
Worker thread started: Worker4 (ID: 09876)
Worker4 is going to sleep...

=== Test 1: Waking up threads by name ===
Waking up thread: Worker1 (ID: 12345)
Worker1 is woken up!
Worker1 is going to sleep...
Waking up thread: Worker2 (ID: 67890)
Worker2 is woken up!
Worker2 is going to sleep...

=== Test 2: Waking up threads by ID ===
Waking up thread: Worker3 (ID: 54321)
Worker3 is woken up!
Worker3 is going to sleep...
Waking up thread: Worker4 (ID: 09876)
Worker4 is woken up!
Worker4 is going to sleep...

=== Test 3: Waking up all threads to exit ===
Waking up thread: Worker1 (ID: 12345)
Worker1 is woken up!
Thread unregistered: Worker1 (ID: 12345)
Worker thread exited: Worker1
Waking up thread: Worker2 (ID: 67890)
Worker2 is woken up!
Thread unregistered: Worker2 (ID: 67890)
Worker thread exited: Worker2
Waking up thread: Worker3 (ID: 54321)
Worker3 is woken up!
Thread unregistered: Worker3 (ID: 54321)
Worker thread exited: Worker3
Waking up thread: Worker4 (ID: 09876)
Worker4 is woken up!
Thread unregistered: Worker4 (ID: 09876)
Worker thread exited: Worker4

Main thread exited
```

## 注意事项

1. **线程注册**：每个线程在使用前必须注册，否则`Sleep()`函数会失败
2. **线程注销**：每个线程在退出前必须注销，否则会导致资源泄漏
3. **异常处理**：系统会捕获并处理pthread函数的错误，但严重错误可能会导致线程退出
4. **信号处理**：系统会正确处理EINTR信号中断，确保线程不会因为信号而错误退出睡眠
5. **线程安全**：所有共享资源都有互斥锁保护，确保线程安全

## 故障排除

1. **编译失败**：检查编译器是否安装，pthread库是否可用
2. **运行时错误**：检查线程是否正确注册，是否在退出前注销
3. **线程无法唤醒**：检查唤醒时使用的线程名或线程ID是否正确
4. **资源泄漏**：确保每个注册的线程都在退出前注销

## 结论

本项目实现了一个跨平台的线程睡眠和唤醒系统，支持Linux和QNX操作系统。系统采用单例模式管理线程，提供了简洁易用的API，同时具备完整的异常处理机制，确保系统的稳定性和可靠性。