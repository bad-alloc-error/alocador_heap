#include<unistd.h>
#include<memory.h>
#include<sys/mman.h>
#include"headers/mmanager.h"

static size_t PAGE_SIZE = {0};

void mmanager_init(){

    PAGE_SIZE = getpagesize();
}

static void* mmanager_page_alloc(int vmp_units){

    char *mem_page = mmap(NULL, vmp_units * PAGE_SIZE, 0x1 | 0x2 | 0x4, MAP_ANON | MAP_PRIVATE, NULL, NULL);


    if(mem_page == MAP_FAILED){

        /*TODO LOGGING */

        return NULL;
    }

    memset(mem_page, 0, vmp_units * PAGE_SIZE);

    return (void *) mem_page;
}

static signed int mmanager_page_dealloc(void* memory_page_addr, int units){

    signed int SUCCESS_RELEASE = munmap(memory_page_addr, units);

    if(!SUCCESS_RELEASE){
        /*TODO LOGGING */

        return -1;
    }

    return SUCCESS_RELEASE;

}
