@echo off

if exist main.exe (
    echo Removing old main.exe...
    del /f /q main.exe
)

echo Building Release version...
g++ main.cpp stack.cpp unitTest.cpp -o main.exe

if %errorlevel% equ 0 (
    echo [SUCCESS] Build completed successfully! Running...
    echo ----------------------------------------
    main.exe
) else (
    echo [ERROR] Build failed.
    pause
)
