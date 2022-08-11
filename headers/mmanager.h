#ifndef _MMANAGER_H_
#define _MMANAGER_H_
#include<stdint.h>

/*Para obter o número máximo de page_family_t que podemos armazenar dentro de uma page
    do tipo vm_page_for_families_t é pegar o tamanho da pagina alocada, subtrair pelo tamanho
    do ponteiro do tipo vm_page_for_families_t e dividir pelo tamanho da objeto vm_page_family_t.
    Na base da página (vm_page_for_families_t) tera um campo onde armazeno o endereço 
    da proxima vm_page_for_families_t e a partir dai as vm_page_family_t serão empilhadas no espaço
    remanescente da página. */

#define MAX_FAMILIES_PER_VM_PAGE \
    (PAGE_SIZE - sizeof(vm_page_for_families_t *)/\
        sizeof(vm_page_family_t))

#define ITER_PAGE_FAMILY_BEGIN(FIRST_PAGE_FOR_FAMILIES_PTR, CURRENT) \
{                                                          \
    uint32_t count_macro = 0;                                  \
    for(CURRENT = (vm_page_family_t *)&FIRST_PAGE_FOR_FAMILIES_PTR->vm_page_family[0]; \
                    CURRENT->size && count_macro < MAX_FAMILIES_PER_VM_PAGE; CURRENT++, count_macro++){ \

#define ITER_PAGE_FAMILY_END(FIRST_PAGE_FOR_FAMILIES_PTR, CURRENT) }}

#define OFFSET_OF(CONTAINER_STRUCT, FIELD_NAME) \
    (CONTAINER_STRUCT, FIELD_NAME)(size_t)(&((CONTAINER_STRUCT *)0)->FIELD_NAME)

#define MMANAGER_GET_PAGE_FROM_META_BLOCK(META_DATA_BLOCK_PTR) \
    ((void *)((char *)META_DATA_BLOCK_PTR - META_DATA_BLOCK_PTR->offset))

#define NEXT_META_BLOCK(META_DATA_BLOCK_PTR) \
    (META_DATA_BLOCK_PTR->next_block)
    
#define NEXT_META_BLOCK_BY_SIZE(META_DATA_BLOCK_PTR) \
    ((block_meta_data_t *)((char *)(META_DATA_BLOCK_PTR + 1) + META_DATA_BLOCK_PTR->block_size))

#define PREV_META_BLOCK(META_DATA_BLOCK_PTR) \
    (META_DATA_BLOCK_PTR->prev_block)

#define MMANAGER_BIND_BLOCKS_FOR_ALLOCATION(ALLOCATED_META_BLOCK, FREE_META_BLOCK) \
    FREE_META_BLOCK->prev_block = ALLOCATED_META_BLOCK \
    FREE_META_BLOCK->next_block = ALLOCATED_META_BLOCK->next_block \
    ALLOCATED_META_BLOCK->next_block = FREE_META_BLOCK \
    FREE_META_BLOCK->block_size = ((PAGE_SIZE - ALLOCATED_META_BLOCK->block_size) - (sizeof(ALLOCATED_META_BLOCK) - sizeof(FREE_META_BLOCK)))/\
    ALLOCATED_META_BLOCK->is_free = MMANAGER_FALSE \     
    if(FREE_META_BLOCK->next_block != NULL) \
    FREE_META_BLOCK->next_block->prev_block = FREE_META_BLOCK
    
#define MAXSIZE_PAGE_FAMILY_NAME 32 

typedef enum{
    MMANAGER_TRUE,
    MMANAGER_FALSE
}vm_bool_t;

typedef struct vm_page_family_{
    char struct_name[MAXSIZE_PAGE_FAMILY_NAME];
    uint32_t size;
}vm_page_family_t;

typedef struct vm_page_for_families_{
    struct vm_page_for_families_* next;
    vm_page_family_t vm_page_family[0];
    uint32_t vm_page_for_families_quantity;
}vm_page_for_families_t;

typedef struct vm_meta_block_data_{
    vm_bool_t is_free;
    uint32_t block_size;
    struct vm_meta_block_data_ *prev_block;
    struct vm_meta_block_data_ *next_block;
    uint64_t offset;
}meta_block_data_t;

void mmanager_init(void);
void mmanager_new_page_family(char* stuct_name, uint32_t size);
void mmanager_print_registered_page_families();
void mmanager_print_meta_blocks_vm_page(meta_block_data_t* meta_block);
static void* mmanager_page_alloc(int vmp_units);
static void* mmanager_merge_free_blocks(meta_block_data_t* first_block, meta_block_data_t* second_block);
static signed int mmanager_page_dealloc(void* memory_page_addr, int units);
vm_page_family_t* lookup_page_family_by_name(char *struct_name);

#endif