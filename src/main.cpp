#include <stdio.h>

#include "unitTest.h"

int main(){
    test_struct_left_canary();
    test_struct_right_canary();
    test_data_left_canary();
    test_data_buffer_overflow();

    test_null_pointer();
    test_data_null();
    test_underflow();
    test_overflow();
    test_bad_capacity();

    test_hash_corrupt();
}
