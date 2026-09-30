#include <stdio.h>
#include <stdlib.h>

struct t{
    int* a;
};

int f(const char* s){
    printf("%s", s);
    return 1;
}

#define a(cap) f(#cap)

int main(){
    int b = a(2);

}
