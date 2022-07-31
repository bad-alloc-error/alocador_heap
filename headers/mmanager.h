#ifndef _MMANAGER_H_
#define _MMANAGER_H_

#define MAX_FAMILIES_PER_VM_PAGE \
    (PAGE_SIZE - sizeof(vm_page_for_families_t *)/\
        sizeof(vm_page_family_t))

typedef struct vm_page_family_{
    char struct_name[MAXSIZE_PAGE_FAMILY_NAME];
    __uint32_t size;
}vm_page_family_t;

typedef struct vm_page_for_families_{
    vm_page_for_families_t next*;
    vm_page_family_t vm_page_family[0];
}vm_page_for_families_t;


void mmanager_init(void);
static void* mmanager_page_alloc(int vmp_units);
static signed int mmanager_page_dealloc(void* memory_page_addr, int units);

#endif