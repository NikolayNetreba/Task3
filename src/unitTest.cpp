#include <stdio.h>

#include "stack.h"
#include "unitTest.h"

void test_struct_left_canary(){
    printf("\n=== TEST 1: Struct Left Canary ===\n");

    stack_t* st = STACK_INIT(10, "test_stack");

    st->leftCanary = 0xBAD1337;

    push_stack(st, 42);

    destroy_stack(st);
}

void test_struct_right_canary(){
    printf("\n=== TEST 2: Struct Right Canary ===\n");

    stack_t* st = STACK_INIT(10, "test_stack");

    st->rightCanary = 0xDEADBEEF;

    push_stack(st, 42);

    destroy_stack(st);
}

void test_data_left_canary(){
    printf("=== TEST 3: Data Left Canary ===\n");

    stack_t* st = STACK_INIT(10, "test_stack");

    canary_t* left_data_canary = (canary_t*)GET_DATA_CANARY_PTR(st);
    *left_data_canary = 0x00;

    push_stack(st, 100);

    destroy_stack(st);
}

void test_data_buffer_overflow(){
    printf("=== TEST 4: Buffer Overflow (Corrupt Data Right Canary) ===\n");

    stack_t* st = STACK_INIT(10, "test_stack");

    size_t dataBytes = sizeof(elem_t) * st->capacity;
    dataBytes += (dataBytes % 8) ? (8 - dataBytes % 8) : 0;

    canary_t* right_data_canary = (canary_t*)(GET_DATA_CANARY_PTR(st) + dataBytes + sizeof(canary_t));
    *right_data_canary = 0x1234567890ABCDEF;

    push_stack(st, 555);

    destroy_stack(st);
}

void test_null_pointer(){
    printf("=== TEST 5: stack null pointer ===\n");

    stack_t* st = NULL;

    push_stack(st, 10);

    destroy_stack(st);
}

void test_data_null(){
    printf("=== TEST 6: Data null pointer ===\n");
    stack_t* st = STACK_INIT(10, "test_stack");

    st->data = NULL;

    push_stack(st, 10);

    destroy_stack(st);
}

void test_underflow(){
    printf("=== TEST 7: underflow ===\n");

    stack_t* st = STACK_INIT(10, "test_stack");
    elem_t val = 0;

    pop_stack(st, &val);

    destroy_stack(st);
}

void test_overflow(){
    printf("=== TEST 8: overflow ===\n");

    stack_t* st = STACK_INIT(10, "test_stack");

    st->size = st->capacity + 4;

    push_stack(st, 10);

    destroy_stack(st);
}

void test_bad_capacity(){
    printf("=== TEST 9: bad capacity ===\n");

    stack_t* st0 = STACK_INIT(-2, "test_1");
    destroy_stack(st0);

    stack_t* st = STACK_INIT(10, "test_2");

    st->capacity = MAX_CAPACITY + 100;
    push_stack(st, 10);

    st->capacity = 0;
    push_stack(st, 10);

    destroy_stack(st);
}

void test_hash_corrupt(){
    printf("=== TEST 10: hash_corrupt ===\n");

    stack_t* st = STACK_INIT(10, "test");

    st->data[4] = 100;
    push_stack(st, 1);

    destroy_stack(st);
}
