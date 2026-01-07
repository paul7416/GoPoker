#include "list.h" 

#define CHECK(expr, msg) \
    if (!(expr)) { \
        fprintf(stderr, "Test failed: %s\n", msg); \
        exit(EXIT_FAILURE); \
    }


void test_uint64_list() {
    printf("Testing uint64_list...\n");
    uint64_list *l = uint64_list_create(3);

    // set/get
    uint64_list_set(l, 0, 500);
    uint64_list_set(l, 1, 400);
    uint64_list_set(l, 2, 300);
    CHECK(uint64_list_get(l, 0) == 500, "uint64_list get/set failed at 0");
    CHECK(uint64_list_get(l, 1) == 400, "uint64_list get/set failed at 1");
    CHECK(uint64_list_get(l, 2) == 300, "uint64_list get/set failed at 2");

    // negative indices
    CHECK(uint64_list_get(l, -1) == 300, "uint64_list negative get -1 failed");
    CHECK(uint64_list_get(l, -2) == 400, "uint64_list negative get -2 failed");
    uint64_list_set(l, -1, 600);
    CHECK(uint64_list_get(l, -1) == 600, "uint64_list negative set -1 failed");

    // append
    CHECK(uint64_list_append(l, 700) == 4, "uint64_list append failed on length");
    CHECK(uint64_list_get(l, l->length - 1) == 700, "uint64_list append failed");

    // insert
    uint64_list_insert(l, 1, 800);
    CHECK(uint64_list_get(l, 1) == 800, "uint64_list insert failed");
    CHECK(uint64_list_get(l, 2) == 400, "uint64_list insert failed");
    CHECK(l->length == 5, "uint64_list insert failed");

    // pop
    uint64_t val = uint64_list_pop(l, -1);
    CHECK(val == 700 && l->length == 4, "uint64_list pop -1 failed");

    // concat
    uint64_list *l2 = uint64_list_create(2);
    uint64_list_set(l2, 0, 900);
    uint64_list_set(l2, 1, 1000);
    uint64_list_concat(l, l2);
    CHECK(uint64_list_get(l, l->length - 2) == 900 && uint64_list_get(l, -1) == 1000, "uint64_list concat failed");

    uint64_list_free(l);
    uint64_list_free(l2);
    printf("uint64_list passed.\n\n");
    
}

void test_uint16_list() {
    printf("Testing uint16_list...\n");
    uint16_list *l = uint16_list_create(3);

    // set/get
    uint16_list_set(l, 0, 500);
    uint16_list_set(l, 1, 400);
    uint16_list_set(l, 2, 300);
    CHECK(uint16_list_get(l, 0) == 500, "uint16_list get/set failed at 0");
    CHECK(uint16_list_get(l, 1) == 400, "uint16_list get/set failed at 1");
    CHECK(uint16_list_get(l, 2) == 300, "uint16_list get/set failed at 2");

    // negative indices
    CHECK(uint16_list_get(l, -1) == 300, "uint16_list negative get -1 failed");
    CHECK(uint16_list_get(l, -2) == 400, "uint16_list negative get -2 failed");
    uint16_list_set(l, -1, 600);
    CHECK(uint16_list_get(l, -1) == 600, "uint16_list negative set -1 failed");

    // append
    CHECK(uint16_list_append(l, 700) == 4, "uint16_list append failed on length");
    CHECK(uint16_list_get(l, l->length - 1) == 700, "uint16_list append failed");

    // insert
    uint16_list_insert(l, 1, 800);
    CHECK(uint16_list_get(l, 1) == 800, "uint16_list insert failed");
    CHECK(uint16_list_get(l, 2) == 400, "uint16_list insert failed");
    CHECK(l->length == 5, "uint16_list insert failed");

    // pop
    uint16_t val = uint16_list_pop(l, -1);
    CHECK(val == 700 && l->length == 4, "uint16_list pop -1 failed");

    // concat
    uint16_list *l2 = uint16_list_create(2);
    uint16_list_set(l2, 0, 900);
    uint16_list_set(l2, 1, 1000);
    uint16_list_concat(l, l2);
    CHECK(uint16_list_get(l, l->length - 2) == 900 && uint16_list_get(l, -1) == 1000, "uint16_list concat failed");

    uint16_list_free(l);
    uint16_list_free(l2);
    printf("uint16_list passed.\n\n");
    
}

void test_uint8_list() {
    printf("Testing uint8_list...\n");
    uint8_list *l = uint8_list_create(3);

    // set/get
    uint8_list_set(l, 0, 50);
    uint8_list_set(l, 1, 40);
    uint8_list_set(l, 2, 30);
    CHECK(uint8_list_get(l, 0) == 50, "uint8_list get/set failed at 0");
    CHECK(uint8_list_get(l, 1) == 40, "uint8_list get/set failed at 1");
    CHECK(uint8_list_get(l, 2) == 30, "uint8_list get/set failed at 2");

    // negative indices
    CHECK(uint8_list_get(l, -1) == 30, "uint8_list negative get -1 failed");
    CHECK(uint8_list_get(l, -2) == 40, "uint8_list negative get -2 failed");
    uint8_list_set(l, -1, 60);
    CHECK(uint8_list_get(l, -1) == 60, "uint8_list negative set -1 failed");

    // append
    CHECK(uint8_list_append(l, 70) == 4, "uint8_list append failed on length");
    CHECK(uint8_list_get(l, l->length - 1) == 70, "uint8_list append failed");

    // insert
    uint8_list_insert(l, 1, 80);
    CHECK(uint8_list_get(l, 1) == 80, "uint8_list insert failed");
    CHECK(uint8_list_get(l, 2) == 40, "uint8_list insert failed");
    CHECK(l->length == 5, "uint8_list insert failed");

    // pop
    uint8_t val = uint8_list_pop(l, -1);
    CHECK(val == 70 && l->length == 4, "uint8_list pop -1 failed");

    // concat
    uint8_list *l2 = uint8_list_create(2);
    uint8_list_set(l2, 0, 90);
    uint8_list_set(l2, 1, 100);
    uint8_list_concat(l, l2);
    CHECK(uint8_list_get(l, l->length - 2) == 90 && uint8_list_get(l, -1) == 100, "uint8_list concat failed");

    uint8_list_free(l);
    uint8_list_free(l2);
    printf("uint8_list passed.\n\n");
    
}

void test_int64_list() {
    printf("Testing int64_list...\n");
    int64_list *l = int64_list_create(3);

    // set/get
    int64_list_set(l, 0, 500);
    int64_list_set(l, 1, 400);
    int64_list_set(l, 2, 300);
    CHECK(int64_list_get(l, 0) == 500, "int64_list get/set failed at 0");
    CHECK(int64_list_get(l, 1) == 400, "int64_list get/set failed at 1");
    CHECK(int64_list_get(l, 2) == 300, "int64_list get/set failed at 2");

    // negative indices
    CHECK(int64_list_get(l, -1) == 300, "int64_list negative get -1 failed");
    CHECK(int64_list_get(l, -2) == 400, "int64_list negative get -2 failed");
    int64_list_set(l, -1, 600);
    CHECK(int64_list_get(l, -1) == 600, "int64_list negative set -1 failed");

    // append
    CHECK(int64_list_append(l, 700) == 4, "int64_list append failed on length");
    CHECK(int64_list_get(l, l->length - 1) == 700, "int64_list append failed");

    // insert
    int64_list_insert(l, 1, 800);
    CHECK(int64_list_get(l, 1) == 800, "int64_list insert failed");
    CHECK(int64_list_get(l, 2) == 400, "int64_list insert failed");
    CHECK(l->length == 5, "int64_list insert failed");

    // pop
    int64_t val = int64_list_pop(l, -1);
    CHECK(val == 700 && l->length == 4, "int64_list pop -1 failed");

    // concat
    int64_list *l2 = int64_list_create(2);
    int64_list_set(l2, 0, 900);
    int64_list_set(l2, 1, 1000);
    int64_list_concat(l, l2);
    CHECK(int64_list_get(l, l->length - 2) == 900 && int64_list_get(l, -1) == 1000, "int64_list concat failed");

    int64_list_free(l);
    int64_list_free(l2);
    printf("int64_list passed.\n\n");
    
}

void test_int32_list() {
    printf("Testing int32_list...\n");
    int32_list *l = int32_list_create(3);

    // set/get
    int32_list_set(l, 0, 500);
    int32_list_set(l, 1, 400);
    int32_list_set(l, 2, 300);
    CHECK(int32_list_get(l, 0) == 500, "int32_list get/set failed at 0");
    CHECK(int32_list_get(l, 1) == 400, "int32_list get/set failed at 1");
    CHECK(int32_list_get(l, 2) == 300, "int32_list get/set failed at 2");

    // negative indices
    CHECK(int32_list_get(l, -1) == 300, "int32_list negative get -1 failed");
    CHECK(int32_list_get(l, -2) == 400, "int32_list negative get -2 failed");
    int32_list_set(l, -1, 600);
    CHECK(int32_list_get(l, -1) == 600, "int32_list negative set -1 failed");

    // append
    CHECK(int32_list_append(l, 700) == 4, "int32_list append failed on length");
    CHECK(int32_list_get(l, l->length - 1) == 700, "int32_list append failed");

    // insert
    int32_list_insert(l, 1, 800);
    CHECK(int32_list_get(l, 1) == 800, "int32_list insert failed");
    CHECK(int32_list_get(l, 2) == 400, "int32_list insert failed");
    CHECK(l->length == 5, "int32_list insert failed");

    // pop
    int32_t val = int32_list_pop(l, -1);
    CHECK(val == 700 && l->length == 4, "int32_list pop -1 failed");

    // concat
    int32_list *l2 = int32_list_create(2);
    int32_list_set(l2, 0, 900);
    int32_list_set(l2, 1, 1000);
    int32_list_concat(l, l2);
    CHECK(int32_list_get(l, l->length - 2) == 900 && int32_list_get(l, -1) == 1000, "int32_list concat failed");

    int32_list_free(l);
    int32_list_free(l2);
    printf("int32_list passed.\n\n");
    
}

void test_int16_list() {
    printf("Testing int16_list...\n");
    int16_list *l = int16_list_create(3);

    // set/get
    int16_list_set(l, 0, 500);
    int16_list_set(l, 1, 400);
    int16_list_set(l, 2, 300);
    CHECK(int16_list_get(l, 0) == 500, "int16_list get/set failed at 0");
    CHECK(int16_list_get(l, 1) == 400, "int16_list get/set failed at 1");
    CHECK(int16_list_get(l, 2) == 300, "int16_list get/set failed at 2");

    // negative indices
    CHECK(int16_list_get(l, -1) == 300, "int16_list negative get -1 failed");
    CHECK(int16_list_get(l, -2) == 400, "int16_list negative get -2 failed");
    int16_list_set(l, -1, 600);
    CHECK(int16_list_get(l, -1) == 600, "int16_list negative set -1 failed");

    // append
    CHECK(int16_list_append(l, 700) == 4, "int16_list append failed on length");
    CHECK(int16_list_get(l, l->length - 1) == 700, "int16_list append failed");

    // insert
    int16_list_insert(l, 1, 800);
    CHECK(int16_list_get(l, 1) == 800, "int16_list insert failed");
    CHECK(int16_list_get(l, 2) == 400, "int16_list insert failed");
    CHECK(l->length == 5, "int16_list insert failed");

    // pop
    int16_t val = int16_list_pop(l, -1);
    CHECK(val == 700 && l->length == 4, "int16_list pop -1 failed");

    // concat
    int16_list *l2 = int16_list_create(2);
    int16_list_set(l2, 0, 900);
    int16_list_set(l2, 1, 1000);
    int16_list_concat(l, l2);
    CHECK(int16_list_get(l, l->length - 2) == 900 && int16_list_get(l, -1) == 1000, "int16_list concat failed");

    int16_list_free(l);
    int16_list_free(l2);
    printf("int16_list passed.\n\n");
    
}

void test_int8_list() {
    printf("Testing int8_list...\n");
    int8_list *l = int8_list_create(3);

    // set/get
    int8_list_set(l, 0, 50);
    int8_list_set(l, 1, 40);
    int8_list_set(l, 2, 30);
    CHECK(int8_list_get(l, 0) == 50, "int8_list get/set failed at 0");
    CHECK(int8_list_get(l, 1) == 40, "int8_list get/set failed at 1");
    CHECK(int8_list_get(l, 2) == 30, "int8_list get/set failed at 2");

    // negative indices
    CHECK(int8_list_get(l, -1) == 30, "int8_list negative get -1 failed");
    CHECK(int8_list_get(l, -2) == 40, "int8_list negative get -2 failed");
    int8_list_set(l, -1, 60);
    CHECK(int8_list_get(l, -1) == 60, "int8_list negative set -1 failed");

    // append
    CHECK(int8_list_append(l, 70) == 4, "int8_list append failed on length");
    CHECK(int8_list_get(l, l->length - 1) == 70, "int8_list append failed");

    // insert
    int8_list_insert(l, 1, 80);
    CHECK(int8_list_get(l, 1) == 80, "int8_list insert failed");
    CHECK(int8_list_get(l, 2) == 40, "int8_list insert failed");
    CHECK(l->length == 5, "int8_list insert failed");

    // pop
    int8_t val = int8_list_pop(l, -1);
    CHECK(val == 70 && l->length == 4, "int8_list pop -1 failed");

    // concat
    int8_list *l2 = int8_list_create(2);
    int8_list_set(l2, 0, 90);
    int8_list_set(l2, 1, 100);
    int8_list_concat(l, l2);
    CHECK(int8_list_get(l, l->length - 2) == 90 && int8_list_get(l, -1) == 100, "int8_list concat failed");

    int8_list_free(l);
    int8_list_free(l2);
    printf("int8_list passed.\n\n");
    
}

void test_double_list() {
    printf("Testing double_list...\n");
    double_list *l = double_list_create(3);

    // set/get
    double_list_set(l, 0, 500);
    double_list_set(l, 1, 400);
    double_list_set(l, 2, 300);
    CHECK(double_list_get(l, 0) == 500, "double_list get/set failed at 0");
    CHECK(double_list_get(l, 1) == 400, "double_list get/set failed at 1");
    CHECK(double_list_get(l, 2) == 300, "double_list get/set failed at 2");

    // negative indices
    CHECK(double_list_get(l, -1) == 300, "double_list negative get -1 failed");
    CHECK(double_list_get(l, -2) == 400, "double_list negative get -2 failed");
    double_list_set(l, -1, 600);
    CHECK(double_list_get(l, -1) == 600, "double_list negative set -1 failed");

    // append
    CHECK(double_list_append(l, 700) == 4, "double_list append failed on length");
    CHECK(double_list_get(l, l->length - 1) == 700, "double_list append failed");

    // insert
    double_list_insert(l, 1, 800);
    CHECK(double_list_get(l, 1) == 800, "double_list insert failed");
    CHECK(double_list_get(l, 2) == 400, "double_list insert failed");
    CHECK(l->length == 5, "double_list insert failed");

    // pop
    double val = double_list_pop(l, -1);
    CHECK(val == 700 && l->length == 4, "double_list pop -1 failed");

    // concat
    double_list *l2 = double_list_create(2);
    double_list_set(l2, 0, 900);
    double_list_set(l2, 1, 1000);
    double_list_concat(l, l2);
    CHECK(double_list_get(l, l->length - 2) == 900 && double_list_get(l, -1) == 1000, "double_list concat failed");

    double_list_free(l);
    double_list_free(l2);
    printf("double_list passed.\n\n");
    
}

void test_bool_list() {
    printf("Testing bool_list...\n");
    bool_list *l = bool_list_create(3);

    // set/get
    bool_list_set(l, 0, 1);
    bool_list_set(l, 1, 1);
    bool_list_set(l, 2, 0);
    CHECK(bool_list_get(l, 0) == 1, "bool_list get/set failed at 0");
    CHECK(bool_list_get(l, 1) == 1, "bool_list get/set failed at 1");
    CHECK(bool_list_get(l, 2) == 0, "bool_list get/set failed at 2");

    // negative indices
    CHECK(bool_list_get(l, -1) == 0, "bool_list negative get -1 failed");
    CHECK(bool_list_get(l, -2) == 1, "bool_list negative get -2 failed");
    bool_list_set(l, -1, 1);
    CHECK(bool_list_get(l, -1) == 1, "bool_list negative set -1 failed");

    // append
    CHECK(bool_list_append(l, 0) == 4, "bool_list append failed on length");
    CHECK(bool_list_get(l, l->length - 1) == 0, "bool_list append failed");

    // insert
    bool_list_insert(l, 1, 0);
    CHECK(bool_list_get(l, 1) == 0, "bool_list insert failed");
    CHECK(bool_list_get(l, 2) == 1, "bool_list insert failed");
    CHECK(l->length == 5, "bool_list insert failed");

    // pop
    bool val = bool_list_pop(l, -1);
    CHECK(val == 0 && l->length == 4, "bool_list pop -1 failed");

    // concat
    bool_list *l2 = bool_list_create(2);
    bool_list_set(l2, 0, 0);
    bool_list_set(l2, 1, 1);
    bool_list_concat(l, l2);
    CHECK(bool_list_get(l, l->length - 2) == 0 && bool_list_get(l, -1) == 1, "bool_list concat failed");

    bool_list_free(l);
    bool_list_free(l2);
    printf("bool_list passed.\n\n");
    
}



int main() { 
test_uint64_list();
test_uint16_list();
test_uint8_list();
test_int64_list();
test_int32_list();
test_int16_list();
test_int8_list();
test_double_list();
test_bool_list();


    printf("All tests passed successfully!\n");
    return 0;
}
