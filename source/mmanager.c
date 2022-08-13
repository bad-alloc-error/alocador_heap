#include<unistd.h>
#include<memory.h>
#include<sys/mman.h>
#include<string.h>
#include<stdio.h>
#include<assert.h>
#include"include/mmanager.h"

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

    char *mem_page = mmap(NULL, vmp_units * PAGE_SIZE, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, NULL, NULL);


    if(mem_page == MAP_FAILED){

        /*TODO LOGGING */
        fprintf(stderr, "Error %s() to alloc %d virtual page memory\n", __FUNCTION__, vmp_units);
        return NULL;
    }

    memset(mem_page, 0, vmp_units * PAGE_SIZE);

    return (void *) mem_page;
}

static signed int mmanager_page_dealloc(void* memory_page_addr, int units){

    signed int SUCCESS_RELEASE = munmap(memory_page_addr, units);

    if(!SUCCESS_RELEASE){
        /*TODO LOGGING */
        fprintf(stderr, "Error %s() to release %d virtual page memory\n", __FUNCTION__, units);
        return -1;
    }

    return SUCCESS_RELEASE;

}

/*retorna o tamanho de data blocks(free) de uma vm page vazia*/
static inline uint32_t mmanager_max_page_allocatable_mem(int units){

    return (uint32_t) ((PAGE_SIZE * units) - OFFSET_OF(vm_page_t, "page_memory"));

}

vm_bool_t mmanager_is_vm_page_empty(vm_page_t* vm_page){

    if(vm_page->meta_block_data.is_free == MMANAGER_TRUE && vm_page->meta_block_data.prev_block == NULL && vm_page->meta_block_data.next_block == NULL) { return MMANAGER_TRUE; } 
    
    return MMANAGER_FALSE;
}

void mmanager_print_meta_blocks_vm_page(meta_block_data_t* first_meta_block){

    meta_block_data_t* meta_block = first_meta_block;
    meta_block_data_t* large_free_block_addr = NULL;
    meta_block_data_t* large_non_free_block_addr = NULL;
    vm_page_for_families_t* first_page_families = first_vm_page_for_families;
    int64_t count_free_blocks, count_non_free_blocks, large_free_block, large_non_free_block = 0;
    

    for(first_page_families; first_page_families != NULL; first_page_families = first_page_families->next){
        
        for(meta_block; meta_block != NULL; meta_block = meta_block->next_block){
            
            if(meta_block->is_free == MMANAGER_TRUE){ 
                count_free_blocks++;
            }else{
                count_non_free_blocks++;
            }

            if(meta_block->block_size > large_free_block && meta_block->is_free == MMANAGER_TRUE){
                large_free_block = meta_block->block_size;
                large_free_block_addr = meta_block;
            }

            if(meta_block->block_size > large_non_free_block && meta_block->is_free == MMANAGER_FALSE){
                large_non_free_block = meta_block->block_size;
                large_non_free_block_addr = meta_block;
            }

        }

    }

    printf("Numbers of free      [meta]blocks: [ %ld ]\n", count_free_blocks);
    printf("Numbers of allocated [meta]blocks: [ %ld ]\n", count_non_free_blocks);
    printf("Address and Size of Largest free block:      [%p] - [%ld]\n", large_free_block_addr, large_free_block);
    printf("Address and Size of Largest allocated block: [%p] - [%ld]\n", large_non_free_block_addr, large_non_free_block);

}

void mmanager_print_registered_page_families(void){

    int count = 0;
    vm_page_for_families_t* first_vm_page = first_vm_page_for_families;
    vm_page_family_t* page_family_current = NULL;

    for(first_vm_page; first_vm_page != NULL; first_vm_page = first_vm_page->next){
        
        ITER_PAGE_FAMILY_BEGIN(first_vm_page, page_family_current){
            printf("Page Family Name: %s - Size: %d\n", first_vm_page->vm_page_family[count].struct_name, first_vm_page->vm_page_family[count].size);
            count++;
        }ITER_PAGE_FAMILY_END(first_vm_page, page_family_current);

    }

}   

static void* mmanager_merge_free_blocks(meta_block_data_t* first_block, meta_block_data_t* second_block){

    if(first_block->is_free = MMANAGER_TRUE && second_block->is_free == MMANAGER_TRUE){
        
        first_block->block_size += sizeof(meta_block_data_t) + second_block->block_size;
        first_block->next_block = second_block->next_block;
        /*checa se o bloco anterior não é o último alocado*/
        if(second_block->next_block){ second_block->next_block->prev_block = first_block; }
        return (void *) first_block;

    }else{

        fprintf(stderr, "%s could not merge data blocks at [%p] and [%p] \n", __FUNCTION__, first_block, second_block);
        return NULL;
    }
}


vm_page_family_t* lookup_page_family_by_name(char* struct_name){

    vm_page_for_families_t* first_vm_page = first_vm_page_for_families;
    vm_page_family_t* page_family_current = NULL;

    for(first_vm_page; first_vm_page != NULL; first_vm_page = first_vm_page->next){

        ITER_PAGE_FAMILY_BEGIN(first_vm_page, page_family_current){

            if(strncmp(page_family_current->struct_name, struct_name, MAXSIZE_PAGE_FAMILY_NAME) == 0){
                
                return page_family_current;
            }
            
        }ITER_PAGE_FAMILY_END(first_vm_page, page_family_current);

    }

    return NULL;

}

void mmanager_new_page_family(char* struct_name, uint32_t size){

     vm_page_family_t* vm_page_family_curr = NULL;
     vm_page_for_families_t* new_vm_page_for_families = NULL;

    if(size > PAGE_SIZE){

        fprintf(stderr, "Error: %s() structure %s size exceeds system page size\n", __FUNCTION__, struct_name);
        return;
    }

    if(!first_vm_page_for_families){

        /* Caso nulo, registra uma nova vm_page_for_families_t (tam: PAGE_SIZE)*/
        first_vm_page_for_families = (vm_page_for_families_t *) mmanager_page_alloc(1);
        first_vm_page_for_families->next = NULL;
        first_vm_page_for_families->vm_page_for_families_quantity++;

        /* Popula a struct */
        strncpy(first_vm_page_for_families->vm_page_family[0].struct_name, struct_name, MAXSIZE_PAGE_FAMILY_NAME);
        first_vm_page_for_families->vm_page_family[0].size = size;

        return;
    }

    uint32_t count = 0;

    /*Itera sobre a Page Family e checa se o número é maior que o permitido de page family na 
        pagina virtual, se for, requer ao kernel uma nova página virtual e armazena a fage family lá.*/

    ITER_PAGE_FAMILY_BEGIN(first_vm_page_for_families, vm_page_family_curr){

        if(strncmp(vm_page_family_curr->struct_name, struct_name, MAXSIZE_PAGE_FAMILY_NAME) != 0){
            count++;
            continue;
        }

    }ITER_PAGE_FAMILY_END(first_vm_page_for_families, vm_page_family_curr);

    if(count == MAX_FAMILIES_PER_VM_PAGE){

        new_vm_page_for_families = (vm_page_for_families_t *)mmanager_page_alloc(1);
        new_vm_page_for_families->next = first_vm_page_for_families;
        new_vm_page_for_families->vm_page_for_families_quantity++;
        first_vm_page_for_families = new_vm_page_for_families;
        vm_page_family_curr = &first_vm_page_for_families->vm_page_family[0];
    }

    strncpy(vm_page_family_curr->struct_name, struct_name, MAXSIZE_PAGE_FAMILY_NAME);
    vm_page_family_curr->size = size;
    
}


