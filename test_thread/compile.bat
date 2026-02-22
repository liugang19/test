@echo off

REM 编译动态链接库
echo Compiling dynamic link library...
g++ -shared -o thread_manager.dll thread_manager.cpp -D DLL_EXPORTS

if %errorlevel% neq 0 (
    echo Failed to compile dynamic link library!
    pause
    exit /b %errorlevel%
)

REM 编译测试程序
echo Compiling test program...
g++ -o test_program.exe test_program.cpp -L. -lthread_manager

if %errorlevel% neq 0 (
    echo Failed to compile test program!
    pause
    exit /b %errorlevel%
)

echo Compilation successful!
echo Run test_program.exe to test the dynamic link library.
pause