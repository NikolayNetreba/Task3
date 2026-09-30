#include <stdio.h>
#include "stack.h"

void colega_pidor(stack_t* a){
    a->size = a->capacity + 4;
}

int main(){
    stack_t* a = STACK_INIT(2, "a");

    push_stack(a, 10);
    push_stack(a, 11);
    DUMP(a, STACK_PRINT);
    elem_t res = 0;
    pop_stack(a, &res);
    ELEM_PRINT(res);
    //colega_pidor(a);

    pop_stack(a, &res);
    ELEM_PRINT(res);
    pop_stack(a, &res);
    pop_stack(a, &res);

    destroy_stack(a);
}
