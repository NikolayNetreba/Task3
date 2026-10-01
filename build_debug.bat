@echo off

if exist main_debug.exe (
    echo Removing old main_debug.exe...
    del /f /q main_debug.exe
)

echo Building Debug version...
g++ src/*.cpp -Iinclude -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE -DDEBUG -o main_debug.exe

if %errorlevel% equ 0 (
    echo [SUCCESS] Build completed successfully! Running...
    echo ----------------------------------------
    main_debug.exe
) else (
    echo [ERROR] Build failed.
    pause
)
