#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "colors.h"
#include "stack.h"

//=============DEBUG======================================
ErrorStack stack_ok(stack_t* a){
    if(a == NULL)
        return STACK_NULL_POINTER;
    if(a->data == NULL)
        return STACK_DATA_NULL;

    if(a->size > a->capacity)
        return STACK_OVERFLOW;
    if(a->capacity == 0 && a->data != NULL)
        return STACK_BAD_CAPACITY;

    return STACK_OK;
}

const char* status_name(ErrorStack err){
    switch(err){
        case STACK_NULL_POINTER:
            return "Null pointer";
        case STACK_OVERFLOW:
            return "Stack overflow (size > capacity)";
        case STACK_UNDERFLOW:
            return "Stack underflow (size < 0)";
        case STACK_DATA_NULL:
            return "Data null";
        case STACK_BAD_CAPACITY:
            return "Bad capacity";
        case STACK_ALLOC_FAILED:
            return "Alloc failed";
        default:
            return "Ok";
    }
}

#ifdef DEBUG
void dump_stack(stack_t* a, ErrorStack err, const char* func, const char* file, int line){
    const char* statName = status_name(err);

    fprintf(stderr, LINE);
    fprintf(stderr, "%s: In function " MAKE_BLUE("%s\n"), file, func);
    fprintf(stderr, "the program crashed in %s:%d: " MAKE_RED("Error status: ") "%s\n", file, line, statName);

    if(err == STACK_NULL_POINTER || err == STACK_DATA_NULL) return;

    fprintf(stderr, "stack_t <" MAKE_GREEN("%s") ">, located at: [%p], created by %s at %s:%d\n{\n", a->name , a, a->func, a->file, a->line);

    fprintf(stderr, "capacity = %zu\nsize     = %zu\ndata[%p]\n", a->capacity, a->size, a->data);
    for(int i = 0; i < a->capacity; i++){
        if(i < a->size){
            fprintf(stderr, "  *[%3d] - ", i);
            ELEM_PRINT(a->data[i]);
        } else {
            fprintf(stderr, "   [%3d] - POISON: ", i);
            ELEM_PRINT(a->data[i]);
            fprintf(stderr, "\n");
        }
    }
    fprintf(stderr, "}\n" LINE);
}
#endif
//=============DEBUG======================================

//=============INITIALIZATION=============================
static ErrorStack fill_data_with_poison(stack_t* outValue){
    if(outValue == NULL) return STACK_NULL_POINTER;

    for(size_t i = outValue->size; i < outValue->capacity; i++){
        outValue->data[i] = ELEM_POISON;
    }

    return STACK_OK;
}

stack_t* init_stack(size_t capacity
                    ON_DBG(, const char* name, const char* func, const char* file, size_t line)){
    assert(capacity < MAX_CAPACITY);


    stack_t* outValue = (stack_t*) calloc(1, sizeof(stack_t));
    if(outValue == NULL){
        return NULL;
    }

    outValue->capacity = capacity;
    outValue->size = 0;

    outValue->data = (elem_t*) calloc(capacity, sizeof(elem_t));
    if(outValue->data == NULL){
        return NULL;
    }

    fill_data_with_poison(outValue);

    #ifdef DEBUG
        outValue->name = name;
        outValue->func = func;
        outValue->file = file;
        outValue->line = line;
    #endif

    return outValue;
}
//=============INITIALIZATION=============================

//=============FUNCTIONS===================================
void destroy_stack(stack_t* a){
    free(a->data);
    free(a);
}

static ErrorStack expand_stack(stack_t* a){
    if(a == NULL) return STACK_NULL_POINTER;

    STACK_CHECK(a);

    size_t newCapacity = (size_t)(a->capacity * 2 + 1);
    elem_t* temp = (elem_t*) realloc(a->data, newCapacity * sizeof(elem_t));
    //temp = NULL;

    if(temp == NULL){
        return STACK_ALLOC_FAILED;
    } else {
        a->data = temp;
        a->capacity = newCapacity;

        fill_data_with_poison(a);
    }

    STACK_CHECK(a);

    return STACK_OK;
}

static ErrorStack narrow_stack(stack_t* a){
    if(a == NULL) return STACK_NULL_POINTER;

    STACK_CHECK(a);

    size_t newCapacity = (size_t)(a->capacity * 0.5 + 1);
    elem_t* temp = (elem_t*)realloc(a->data, newCapacity * sizeof(elem_t));

    if(temp == NULL){
        return STACK_ALLOC_FAILED;
    } else {
        a->data = temp;
        a->capacity = newCapacity;

        fill_data_with_poison(a);
    }

    STACK_CHECK(a);

    return STACK_OK;
}

ErrorStack push_stack(stack_t* a, elem_t elem){
    if(a == NULL) return STACK_NULL_POINTER;

    STACK_CHECK(a);

    if(a->capacity == a->size){
        DUMP(a, expand_stack(a));
    }

    a->data[a->size++] = elem;

    STACK_CHECK(a);

    return STACK_OK;
}

ErrorStack pop_stack(stack_t* a, elem_t* outValue){
    if(a == NULL || outValue == NULL) return STACK_NULL_POINTER;

    STACK_CHECK(a);

    if(a->size == 0){
        DUMP(a, STACK_UNDERFLOW);
        return STACK_UNDERFLOW;
    }

    if(a->size * 4 <= a->capacity && a->capacity > MIN_CAPACITY){
        DUMP(a, narrow_stack(a));
    }

    *outValue = a->data[--a->size];
    a->data[a->size] = ELEM_POISON;

    STACK_CHECK(a);

    return STACK_OK;
}
//=============FUNCTIONS===================================














