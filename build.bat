@echo off

if exist main.exe (
    echo Removing old main.exe...
    del /f /q main.exe
)

echo Building Release version...
g++ src/*.cpp -Iinclude

if %errorlevel% equ 0 (
    echo [SUCCESS] Build completed successfully! Running...
    echo ----------------------------------------
    main.exe
) else (
    echo [ERROR] Build failed.
    pause
)
