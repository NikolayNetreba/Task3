#pragma once

#include <stdint.h>
#include "config.h"

//======= define | typedef ===================

#define LINE "--------------------------------------------------\n"

#define MIN_CAPACITY 10
#define MAX_CAPACITY 1073741824

typedef uint64_t canary_t;
#define CANARY_VALUE 0x000D0AFFDEADBEEF

#define GET_DATA_CANARY_PTR(st) ((char*)(st)->data - sizeof(canary_t))

//========== enum | struct ===================

typedef enum{
    STACK_OK = 0,
    STACK_NULL_POINTER,
    STACK_DATA_NULL,
    STACK_UNDERFLOW,
    STACK_OVERFLOW,
    STACK_BAD_CAPACITY,
    STACK_ALLOC_FAILED,
    STACK_PRINT,
    STACK_CANARY_DIED,
    STACK_DATA_CANARY_DIED
} ErrorStack;

typedef struct{
    canary_t leftCanary;

    elem_t* data;
    size_t size;
    size_t capacity;

    #ifdef DEBUG
        const char* name;
        const char* func;
        const char* file;
        size_t line;
    #endif

    canary_t rightCanary;
} stack_t;

//============ prototype ==================

#ifdef DEBUG
    #define ON_DBG(...) __VA_ARGS__
    stack_t* init_stack(size_t capacity, const char* name, const char* func, const char* file, size_t line);
    #define STACK_INIT(capacity, name) init_stack(capacity ON_DBG(, name, __func__, __FILE__, __LINE__))
#else
    #define ON_DBG(...)
    stack_t* init_stack(size_t capacity);
    #define STACK_INIT(capacity, name) init_stack(capacity)
#endif



void destroy_stack(stack_t* a);
ErrorStack push_stack(stack_t* a, elem_t elem);
ErrorStack pop_stack(stack_t* a, elem_t* outValue);

const char* status_name(ErrorStack err);
ErrorStack stack_ok(stack_t* a);

//================= debug ===========================

void dump_stack(stack_t* a, ErrorStack err, const char* func, const char* file, int line);

#ifdef DEBUG
    #define DUMP(st, status) do{                         \
        dump_stack(st, status, __func__, __FILE__, __LINE__);  \
    } while(0)

    #define DUMP_RETURN(st, status) do{\
        if(status != STACK_OK){\
            dump_stack(st, status, __func__, __FILE__, __LINE__);\
            return status;\
        }\
    } while(0)

    #define STACK_CHECK(st) do{                          \
        ErrorStack status = stack_ok(st);                \
        if(status != STACK_OK){                          \
            dump_stack(st, status, __func__, __FILE__, __LINE__);  \
            return status;                               \
        }                                                \
    } while(0)
#else
    #define DUMP(st, status)

    #define DUMP_RETURN(st, status)

    #define STACK_CHECK(st) do{            \
        ErrorStack status = stack_ok(st);  \
        if(status != STACK_OK){            \
            return status;                 \
        }                                  \
    } while(0)
#endif



