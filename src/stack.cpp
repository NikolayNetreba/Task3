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
static uint64_t calc_hash(const void* ptr, size_t len, uint64_t startHash){
    const uint8_t* oneByte = (const uint8_t*)ptr;
    uint64_t hash = startHash;

    for(size_t i = 0; i < len; i++){
        hash = (hash << 6) + hash + oneByte[i];
    }

    return hash;
}

static uint64_t calc_full_stack_hash(stack_t* stack){
    assert(stack);

    stack_t temp = *stack;
    temp.hash = 0;

    uint64_t h = calc_hash(&temp, sizeof(stack_t), START_HASH);

    if(temp.data != NULL){
        h = calc_hash(GET_DATA_CANARY_PTR(stack), 2 * sizeof(canary_t) + sizeof(elem_t) * stack->capacity, h);
    }

    return h;
}

ErrorStack stack_ok(stack_t* stack){
    if(stack == NULL)
        return STACK_NULL_POINTER;
    if(stack->data == NULL)
        return STACK_DATA_NULL;

    if(stack->capacity > MAX_CAPACITY || (stack->capacity == 0 && stack->data != NULL))
        return STACK_BAD_CAPACITY;
    if(stack->size > stack->capacity)
        return STACK_OVERFLOW;

    if(stack->leftCanary != CANARY_VALUE || stack->rightCanary != CANARY_VALUE)
        return STACK_CANARY_DIED;
    if(*(canary_t*)(GET_DATA_CANARY_PTR(stack)) != CANARY_VALUE || *find_right_canary(GET_DATA_CANARY_PTR(stack), stack->capacity) != CANARY_VALUE)
        return STACK_DATA_CANARY_DIED;

    if(stack->hash != calc_full_stack_hash(stack)){
        return STACK_HASH_WAS_CORRUPTED;
    }

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
        case STACK_HASH_WAS_CORRUPTED:
            return "Hash was corrupted";
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

void dump_stack(stack_t* stack, ErrorStack err, const char* func, const char* file, int line){
    const char* statName = status_name(err);

    fprintf(stderr, LINE);
    fprintf(stderr, "%s: In function " MAKE_BLUE("%s\n"), file, func);
    fprintf(stderr, "the program crashed in %s:%d: " MAKE_RED("Error status: ") "%s\n", file, line, statName);

    if(err == STACK_NULL_POINTER || err == STACK_DATA_NULL) {
        fprintf(stderr, LINE);
        return;
    }
    if(err == STACK_BAD_CAPACITY){
        fprintf(stderr, "capacity = %zu\n" LINE, stack->capacity);
        return;
    }

    fprintf(stderr, "stack_t <" MAKE_GREEN("%s") ">, located at: [%p], created by %s at %s:%zu\n{\n", stack->name , stack, stack->func, stack->file, stack->line);

    fprintf(stderr, "capacity = %zu\nsize     = %zu\ndata[%p]\n", stack->capacity, stack->size, stack->data);

    print_canary(stack->leftCanary, "stack left canary");
    print_canary(stack->rightCanary, "stack right canary");
    print_canary(*(canary_t*)(GET_DATA_CANARY_PTR(stack)), "data left canary");
    print_canary(*find_right_canary(GET_DATA_CANARY_PTR(stack), stack->capacity), "data right canary");
    if(err == STACK_HASH_WAS_CORRUPTED){
        fprintf(stderr, "\ncurrent hash = " MAKE_RED("%llu") "\nexpected hash = %llu\n", calc_full_stack_hash(stack), stack->hash);
    } else {
        fprintf(stderr, "\nhash = %llu\n", stack->hash);
    }

    for(size_t i = 0; i < stack->capacity; i++){
        if(i < stack->size){
            fprintf(stderr, "  *[%3zu] - ", i);
            ELEM_PRINT(stack->data[i]);
        } else {
            fprintf(stderr, "   [%3zu] - POISON: ", i);
            ELEM_PRINT(stack->data[i]);
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
        stack->hash = calc_full_stack_hash(stack);
    #endif

    return stack;
}
//=============INITIALIZATION=============================

//=============FUNCTIONS===================================
void destroy_stack(stack_t* stack){
    if(stack == NULL || stack->data == NULL) return;

    free(GET_DATA_CANARY_PTR(stack));
    free(stack);
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

static ErrorStack expand_stack(stack_t* stack){
    STACK_CHECK(stack);

    size_t newCapacity = (size_t)(stack->capacity * 2 + 1);
    elem_t* temp = realloc_data(GET_DATA_CANARY_PTR(stack), newCapacity);

    if(temp == NULL){
        return STACK_ALLOC_FAILED;
    } else {
        stack->data = temp;
        stack->capacity = newCapacity;

        fill_data_with_poison(stack);
    }

    STACK_CHECK(stack);

    return STACK_OK;
}

static ErrorStack narrow_stack(stack_t* stack){
    STACK_CHECK(stack);

    size_t newCapacity = (size_t)(stack->capacity / 2 + 1);
    elem_t* temp = realloc_data(GET_DATA_CANARY_PTR(stack), newCapacity);

    if(temp == NULL){
        return STACK_ALLOC_FAILED;
    } else {
        stack->data = temp;
        stack->capacity = newCapacity;

        fill_data_with_poison(stack);
    }

    STACK_CHECK(stack);

    return STACK_OK;
}

ErrorStack push_stack(stack_t* stack, elem_t elem){
    STACK_CHECK(stack);

    if(stack->capacity == stack->size){
        DUMP_RETURN(stack, expand_stack(stack));
    }

    stack->data[stack->size++] = elem;

    #ifdef DEBUG
    stack->hash = calc_full_stack_hash(stack);
    #endif

    STACK_CHECK(stack);

    return STACK_OK;
}

ErrorStack pop_stack(stack_t* stack, elem_t* outValue){
    if(outValue == NULL) return STACK_NULL_POINTER;

    STACK_CHECK(stack);

    if(stack->size == 0){
        DUMP_RETURN(stack, STACK_UNDERFLOW);
    }

    if(stack->size * 4 <= stack->capacity && stack->capacity > MIN_CAPACITY){
        DUMP_RETURN(stack, narrow_stack(stack));
    }

    *outValue = stack->data[--stack->size];
    stack->data[stack->size] = ELEM_POISON;

    #ifdef DEBUG
    stack->hash = calc_full_stack_hash(stack);
    #endif

    STACK_CHECK(stack);

    return STACK_OK;
}
//=============FUNCTIONS===================================














