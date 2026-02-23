@echo off

REM 動的リンクライブラリをコンパイル
echo Compiling dynamic link library...
g++ -shared -o thread_manager_jp.dll thread_manager_jp.cpp -D DLL_EXPORTS

if %errorlevel% neq 0 (
    echo Failed to compile dynamic link library!
    pause
    exit /b %errorlevel%
)

REM テストプログラムをコンパイル
echo Compiling test program...
g++ -o test_program_jp.exe test_program_jp.cpp -L. -lthread_manager_jp

if %errorlevel% neq 0 (
    echo Failed to compile test program!
    pause
    exit /b %errorlevel%
)

echo Compilation successful!
echo Run test_program_jp.exe to test the dynamic link library.
pause