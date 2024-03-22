#include "mem.h"
#include "mem_internals.h"
#include "test/test.h"
#include "util.h"

int main(){
    heap_init(REGION_MIN_SIZE);
    test_1();
    test_2();
    test_3();
    test_4();
    test_5();
}
