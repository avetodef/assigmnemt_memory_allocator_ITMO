#include "mem.h"
#include "mem_internals.h"
#include "test.h"
#include <unistd.h>

#define BLOCK_MIN_CAPACITY 24
#define QUERY_1 10
#define QUERY_2 100
#define QUERY_3 200
#define QUERY_TO_EXPAND 10000
#define QUERY_EXPANDING 20000

extern struct block_header *block_get_header(void *contents);

bool test_1()
{

    puts("----test 1: basic sucsessful freeing of memory----\n");

    void *mem1 = _malloc(QUERY_1);
    void *mem2 = _malloc(QUERY_2);

    struct block_header *block1 = block_get_header(mem1);
    struct block_header *block2 = block_get_header(mem2);

    if (block1->is_free || block1->capacity.bytes != BLOCK_MIN_CAPACITY)
    {
        fputs("\033[0;31m", stderr);
        fputs("Incorrect allocation. Capacity.bytes must be 24, TEST 1 FAILED\n", stderr);
        fputs("\033[0;37m", stderr);
        return false;
    }
    if (block2->is_free || block2->capacity.bytes != QUERY_2)
    {
        fputs("\033[0;31m", stderr);
        fputs("Incorrect allocation. Capacity.bytes must be 100, TEST 1 FAILED\n", stderr);
        fputs("\033[0;37m", stderr);
        return false;
    }

    _free(mem1);
    _free(mem2);

    fputs("\033[0;32m", stdout);
    
    fputs("----TEST 1 PASSED----\n", stdout);

    fputs("\033[0;37m", stderr);
    return true;
}

bool test_2()
{

    puts("\n----test 2: freeing only one block out of two----\n");

    void *mem1 = _malloc(QUERY_1);
    void *mem2 = _malloc(QUERY_2);

    struct block_header *block1 = block_get_header(mem1);
    struct block_header *block2 = block_get_header(mem2);

    _free(mem1);

    if (!block1->is_free)
    {
        fputs("\033[0;31m", stderr);
        fputs("memory is not free, test failed\n", stderr);
        fputs("\033[0;37m", stderr);
        return false;
    }
    if (block2->is_free)
    {
        fputs("\033[0;31m", stderr);
        fputs("memory_2 is free, but it shouldn't, test failed\n", stderr);
        fputs("\033[0;37m", stderr);
        return false;
    }

    fputs("\033[0;32m", stdout);
    fputs("----TEST 2 PASSED----\n", stdout);

    _free(mem2);

    fputs("\033[0;37m", stderr);
    return true;
}

bool test_3()
{

    fputs("\n---test 3: freeing two block out of three---\n", stdout);

    void *mem1 = _malloc(QUERY_1);
    void *mem2 = _malloc(QUERY_2);
    void *mem3 = _malloc(QUERY_3);

    struct block_header *block1 = block_get_header(mem1);
    struct block_header *block2 = block_get_header(mem2);
    struct block_header *block3 = block_get_header(mem3);

    _free(mem2);

    if (!block2->is_free)
    {
        fputs("\033[0;31m", stderr);
        fputs("memory is not free, test failed\n", stderr);
        fputs("\033[0;37m", stderr);
        return false;
    }
    if (block1->is_free || block3->is_free)
    {
        fputs("\033[0;31m", stderr);
        fputs("memory_1  or memory_3 is free, but it shouldn't, test failed\n", stderr);
        fputs("\033[0;37m", stderr);
        return false;
    }

    fputs("\033[0;32m", stdout);
    fputs("---TEST 3 PASSED----\n", stdout);

    _free(mem1);
    _free(mem3);

    fputs("\033[0;37m", stderr);
    return true;
}

bool test_4()
{

    fputs("\n----test 4: expanding regions----\n", stdout);

    void *mem1 = _malloc(QUERY_TO_EXPAND);
    void *mem2 = _malloc(QUERY_EXPANDING);

    struct block_header *block1 = block_get_header(mem1);
    struct block_header *block2 = block_get_header(mem2);

    if (block1->next != block2)
    {
        fputs("\033[0;31m", stderr);
        fputs("blocks are not connected, test failed\n", stderr);
        fputs("\033[0;37m", stderr);
        return false;
    }

    _free(mem1);
    _free(mem2);

    fputs("\033[0;32m", stdout);
    fputs("----TEST 4 PASSED----\n", stdout);

    fputs("\033[0;37m", stderr);
    return true;
}

bool test_5()
{
    fputs("\n----test 5: complex region expanding----\n", stdout);

    void *mem1 = _malloc(QUERY_TO_EXPAND);
    struct block_header *block1 = block_get_header(mem1);

    while (block1->next != NULL)
    {
        block1 = block1->next;
    }

    mmap(block1, QUERY_TO_EXPAND, PROT_READ | PROT_WRITE, MAP_FIXED, 0, 0);

    void *mem2 = _malloc(QUERY_EXPANDING);
    struct block_header *block2 = block_get_header(mem2);

    if (block1->contents + block1->capacity.bytes == (uint8_t *)block2)
    {
        fputs("\033[0;31m", stderr);
        fputs("regions are continuously mapped, test failed\n", stderr);
        return false;
    }

    fputs("\033[0;32m", stdout);
    fputs("----TEST 5 PASSED----\n", stdout);

    fputs("\033[0;37m", stderr);
    return true;
}
