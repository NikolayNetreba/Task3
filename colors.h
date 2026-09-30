#ifndef COLORS_H
#define COLORS_H
// цвета для консоли
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

#define MAKE_RED(A)     RED    A RESET
#define MAKE_GREEN(A)   GREEN  A RESET
#define MAKE_YELLOW(A)  YELLOW A RESET
#define MAKE_BLUE(A)    BLUE   A RESET
#define MAKE_MAGENTA(A) MAGENTA A RESET
#define MAKE_CYAN(A)    CYAN A RESET

#endif
