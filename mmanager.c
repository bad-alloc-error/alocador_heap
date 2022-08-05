#include<unistd.h>
#include<memory.h>
#include<sys/mman.h>
#include<string.h>
#include<stdio.h>
#include"headers/mmanager.h"

static size_t PAGE_SIZE = 0;

/* Aponta para o objeto instanciado mais recentemente.
    
    1- Instancia uma nova vm_page_family_t.

    2- Se o gerenciador de memória ainda não tiver nenhuma 
    página virtual alocada (vm_page_for_families_t), então
    ele requer uma página para o kernel e atualiza a variavel
    first_vm_page_for_families apontando para um página válida.

    3- Verifica se a nova vm_page_family_t pode ser adicionada a first_vm_page_for_families_t.
        3.1- Se caso ela pode ser adicionada, então vm_page_family_t é
             adicionada no array vm_page_family da estrutura vm_page_for_families_t.
                3.2- No caso de não entrada então o gerenciador requer uma nova
                página para o kernel, atualiza o ponteiro first_vm_page_for_families
                para a página alocada mais recentemente.
*/
static vm_page_for_families_t *first_vm_page_for_families = NULL;

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

void mmanager_new_page_family(char* struct_name, uint32_t size){

     vm_page_family_t* vm_page_family_curr = NULL;
     vm_page_for_families_t* new_vm_page_for_families = NULL;

    if(size > PAGE_SIZE){

        printf("Error: %s() structure %s size exceeds system page size\n", __FUNCTION__, struct_name);
        return;
    }

    if(!first_vm_page_for_families){

        /* Caso nulo, registra uma nova vm_page_for_families_t (tam: PAGE_SIZE)*/
        first_vm_page_for_families = (vm_page_for_families_t *) mmanager_page_alloc(1);
        first_vm_page_for_families->next = NULL;

        /* Popula a struct */
        strncpy(first_vm_page_for_families->vm_page_family[0].struct_name, struct_name, MAXSIZE_PAGE_FAMILY_NAME);
        first_vm_page_for_families->vm_page_family[0].size = size;

        return;
    }

    uint32_t count = 0;

    ITER_PAGE_FAMILY_BEGIN(first_vm_page_for_families, vm_page_family_curr){

        if(strncmp(vm_page_family_curr->struct_name, struct_name, MAXSIZE_PAGE_FAMILY_NAME) != 0){
            count++;
            continue;
        }

    }ITER_PAGE_FAMILY_END(first_vm_page_for_families, vm_page_family_curr);

    if(count == MAX_FAMILIES_PER_VM_PAGE){

        new_vm_page_for_families = (vm_page_for_families_t *)mmanager_page_alloc(1);
        new_vm_page_for_families->next = first_vm_page_for_families;
        first_vm_page_for_families = new_vm_page_for_families;
        vm_page_family_curr = &first_vm_page_for_families->vm_page_family[0];
    }

    strncpy(vm_page_family_curr->struct_name, struct_name, MAXSIZE_PAGE_FAMILY_NAME);
    vm_page_family_curr->size = size;
    
}


