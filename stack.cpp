#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "colors.h"
#include "stack.h"

static size_t calc_data_bytes(size_t capacity){
    size_t dataBytes = sizeof(elem_t) * capacity;
    dataBytes += (dataBytes % 8) ? (8 - dataBytes % 8) : 0;//align to 8

    return dataBytes;
}

static canary_t* find_right_canary(char* start, size_t capacity){
    size_t dataBytes = calc_data_bytes(capacity);

    return (canary_t*)(start + dataBytes + sizeof(canary_t));
}

//=============DEBUG======================================
ErrorStack stack_ok(stack_t* a){
    if(a == NULL)
        return STACK_NULL_POINTER;
    if(a->data == NULL)
        return STACK_DATA_NULL;

    if(a->capacity > MAX_CAPACITY || (a->capacity == 0 && a->data != NULL))
        return STACK_BAD_CAPACITY;
    if(a->size > a->capacity)
        return STACK_OVERFLOW;

    if(a->leftCanary != CANARY_VALUE || a->rightCanary != CANARY_VALUE)
        return STACK_CANARY_DIED;
    if(*(canary_t*)(GET_DATA_CANARY_PTR(a)) != CANARY_VALUE || *find_right_canary(GET_DATA_CANARY_PTR(a), a->capacity) != CANARY_VALUE)
        return STACK_DATA_CANARY_DIED;

    return STACK_OK;
}

const char* status_name(ErrorStack err){
    switch(err){
        case STACK_CANARY_DIED:
            return "STACK STRUCT CANARY DEAD: Someone corrupted the stack memory structure";
        case STACK_DATA_CANARY_DIED:
            return "STACK DATA CANARY DEAD: Buffer overflow or underflow in elements array";
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
        case STACK_OK:
            return "OK";
        case STACK_PRINT:
            return "it is just for print";
        default:
            return "shouldn't be here";
    }
}

#ifdef DEBUG
static void print_canary(uint64_t canary, const char* label){
    bool is_ok = (canary == CANARY_VALUE);

    const char* prefix = is_ok ? "    " : ">>> ";

    fprintf(stderr, "%s %-18s = 0x%016llX ", prefix, label, canary);
    if (is_ok) {
        fprintf(stderr, "[" MAKE_GREEN("OK") "]\n");
    } else {
        fprintf(stderr, "[" MAKE_RED("DEAD!") "]\n");
        fprintf(stderr, "                Expected: 0x%016llX\n", (uint64_t)CANARY_VALUE);
    }
}

void dump_stack(stack_t* a, ErrorStack err, const char* func, const char* file, int line){
    const char* statName = status_name(err);

    fprintf(stderr, LINE);
    fprintf(stderr, "%s: In function " MAKE_BLUE("%s\n"), file, func);
    fprintf(stderr, "the program crashed in %s:%d: " MAKE_RED("Error status: ") "%s\n", file, line, statName);

    if(err == STACK_NULL_POINTER || err == STACK_DATA_NULL) {
        fprintf(stderr, LINE);
        return;
    }
    if(err == STACK_BAD_CAPACITY){
        fprintf(stderr, "capacity = %zu\n" LINE, a->capacity);
        return;
    }

    fprintf(stderr, "stack_t <" MAKE_GREEN("%s") ">, located at: [%p], created by %s at %s:%zu\n{\n", a->name , a, a->func, a->file, a->line);

    fprintf(stderr, "capacity = %zu\nsize     = %zu\ndata[%p]\n", a->capacity, a->size, a->data);

    print_canary(a->leftCanary, "stack left canary");
    print_canary(a->rightCanary, "stack right canary");
    print_canary(*(canary_t*)(GET_DATA_CANARY_PTR(a)), "data left canary");
    print_canary(*find_right_canary(GET_DATA_CANARY_PTR(a), a->capacity), "data right canary");

    fprintf(stderr, "\n");
    for(size_t i = 0; i < a->capacity; i++){
        if(i < a->size){
            fprintf(stderr, "  *[%3zu] - ", i);
            ELEM_PRINT(a->data[i]);
        } else {
            fprintf(stderr, "   [%3zu] - POISON: ", i);
            ELEM_PRINT(a->data[i]);
        }
    }
    fprintf(stderr, "}\n" LINE);
}
#endif
//=============DEBUG======================================

//=============INITIALIZATION=============================
static ErrorStack fill_data_with_poison(stack_t* stack){
    if(stack == NULL) return STACK_NULL_POINTER;

    for(size_t i = stack->size; i < stack->capacity; i++){
        stack->data[i] = ELEM_POISON;
    }

    return STACK_OK;
}

static elem_t* data_allocation(size_t capacity){
    size_t dataBytes = calc_data_bytes(capacity);
    size_t totalBytes = dataBytes + sizeof(canary_t) * 2;

    char* data = (char*) calloc(totalBytes, 1);
    if(data == NULL){
        return NULL;
    }

    *(canary_t*)data = CANARY_VALUE;

    *find_right_canary(data, capacity) = CANARY_VALUE;

    return (elem_t*)(data + sizeof(canary_t));
}

stack_t* init_stack(size_t capacity
                    ON_DBG(, const char* name, const char* func, const char* file, size_t line)){

    if(capacity > MAX_CAPACITY) return NULL;

    stack_t* stack = (stack_t*) calloc(1, sizeof(stack_t));
    if(stack == NULL){
        DUMP(stack, STACK_NULL_POINTER);
        return NULL;
    }

    stack->capacity = capacity;
    stack->size = 0;
    stack->leftCanary = CANARY_VALUE;
    stack->rightCanary = CANARY_VALUE;

    stack->data = data_allocation(capacity);
    if(stack->data == NULL){
        DUMP(stack, STACK_DATA_NULL);
        destroy_stack(stack);
        return NULL;
    }

    fill_data_with_poison(stack);

    #ifdef DEBUG
        stack->name = name;
        stack->func = func;
        stack->file = file;
        stack->line = line;
    #endif

    return stack;
}
//=============INITIALIZATION=============================

//=============FUNCTIONS===================================
void destroy_stack(stack_t* a){
    if(a == NULL || a->data == NULL) return;

    free(GET_DATA_CANARY_PTR(a));
    free(a);
}

static elem_t* realloc_data(char* data, size_t newCapacity){
    if (data == NULL) return NULL;

    size_t dataBytes = calc_data_bytes(newCapacity);
    size_t totalBytes = dataBytes + sizeof(canary_t) * 2;

    char* temp = (char*) realloc(data, totalBytes);
    if(temp == NULL){
        return NULL;
    }

    canary_t* rightCanary = find_right_canary(temp, newCapacity);
    *rightCanary = CANARY_VALUE;

    return (elem_t*)(temp + sizeof(canary_t));
}

static ErrorStack expand_stack(stack_t* a){
    STACK_CHECK(a);

    size_t newCapacity = (size_t)(a->capacity * 2 + 1);
    elem_t* temp = realloc_data(GET_DATA_CANARY_PTR(a), newCapacity);

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
    STACK_CHECK(a);

    size_t newCapacity = (size_t)(a->capacity / 2 + 1);
    elem_t* temp = realloc_data(GET_DATA_CANARY_PTR(a), newCapacity);

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
    STACK_CHECK(a);

    if(a->capacity == a->size){
        DUMP_RETURN(a, expand_stack(a));
    }

    a->data[a->size++] = elem;

    STACK_CHECK(a);

    return STACK_OK;
}

ErrorStack pop_stack(stack_t* a, elem_t* stack){
    if(stack == NULL) return STACK_NULL_POINTER;

    STACK_CHECK(a);

    if(a->size == 0){
        DUMP_RETURN(a, STACK_UNDERFLOW);
    }

    if(a->size * 4 <= a->capacity && a->capacity > MIN_CAPACITY){
        DUMP_RETURN(a, narrow_stack(a));
    }

    *stack = a->data[--a->size];
    a->data[a->size] = ELEM_POISON;

    STACK_CHECK(a);

    return STACK_OK;
}
//=============FUNCTIONS===================================














