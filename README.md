[![Language](https://img.shields.io/badge/language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))

[![GitHub forks](https://img.shields.io/github/forks/NikolayNetreba/Task3)](https://github.com/NikolayNetreba/Task3/network/members)
[![GitHub stars](https://img.shields.io/github/stars/NikolayNetreba/Task3)](https://github.com/NikolayNetreba/Task3/stargazers)
[![GitHub pull requests](https://img.shields.io/github/issues-pr/NikolayNetreba/Task3)](https://github.com/NikolayNetreba/Task3/pulls)
[![GitHub last commit](https://img.shields.io/github/last-commit/NikolayNetreba/Task3)](https://github.com/NikolayNetreba/Task3/commits/main)

# Task Stack

<img src="https://miro.medium.com/v2/resize:fit:1000/0*s4dwDR8AEY-P5zHP.jpg" style="width: 400px; image-rendering: pixelated;" alt="Четкое изображение">

# Постановка задачи:
- :white_check_mark: Реализовать структуру данных стек
- :white_check_mark: Сделать полную валидацию ошибок(overflow, underflow, null pointer и.т.д)
- :white_check_mark: Сделать отладочный режим, который вызывает DUMP стека в случае ошибки
- :white_check_mark: Реализовать канареечную защиту для всего стека и для массива где хранятся элементы
- :white_square_button: Реализовать Защиту стека с помощью хэширования

# Общая архитектура
```text
.
├── colors.h
├── config.h
├── stack.h
├── unitTest.h
|
├── main.cpp
├── stack.cpp
└── unitTest.cpp
```

Выбор типа данных, формат вывода, значение по умолчанию задаются в ```config.h```. В ```stack.h``` описана логика стека и отладочного режима.В ```unitTest``` описаны юнит-тесты.

# Компиляция и запуск
- ```g++ main.cpp stack.cpp unitTest.cpp``` - стандартный запуск с юнит-тестами.
- Чтобы включить отладочный режим нужно использовать флаг: ```-DDEBUG```

# Фичи

# colors.h
### Возможные цвета:
```cpp
MAKE_RED(text)
MAKE_GREEN(text)
MAKE_YELLOW(text)
MAKE_BLUE(text)
MAKE_MAGENTA(text)
MAKE_CYAN(text)
```
Пример использования:
```cpp
fprintf(stderr, MAKE_YELLOW("Enter the file name:"));
```
