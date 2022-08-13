#ifndef _MMANAGER_H_
#define _MMANAGER_H_
#include<stdint.h>
#include"struct_declare.h"


#define LOG_ERROR(msg, ...) \
    fprintf(stderr, msg, __VA_ARGS__)

/*Para obter o número máximo de page_family_t que podemos armazenar dentro de uma page
    do tipo vm_page_for_families_t é pegar o tamanho da pagina alocada, subtrair pelo tamanho
    do ponteiro do tipo vm_page_for_families_t e dividir pelo tamanho da objeto vm_page_family_t.
    Na base da página (vm_page_for_families_t) tera um campo onde armazeno o endereço 
    da proxima vm_page_for_families_t e a partir dai as vm_page_family_t serão empilhadas no espaço
    remanescente da página. */

#define MAX_FAMILIES_PER_VM_PAGE \
    (PAGE_SIZE - sizeof(vm_page_for_families_t *)/\
        sizeof(vm_page_family_t))

typedef enum vm_bool_{
    MMANAGER_TRUE,
    MMANAGER_FALSE
}vm_bool_t;

#define ITER_PAGE_FAMILY_BEGIN(FIRST_PAGE_FOR_FAMILIES_PTR, CURRENT) \
{                                                          \
    uint32_t count_macro = 0;                                  \
    for(CURRENT = (vm_page_family_t *)&FIRST_PAGE_FOR_FAMILIES_PTR->vm_page_family[0]; \
                    CURRENT->size && count_macro < MAX_FAMILIES_PER_VM_PAGE; CURRENT++, count_macro++){ \

#define ITER_PAGE_FAMILY_END(FIRST_PAGE_FOR_FAMILIES_PTR, CURRENT) }}

/*VM_PAGE_FAMILY_PTR = vm_page_family_t | CURRENT = vm_page_t*/
#define ITER_VM_PAGE_BEGIN(VM_PAGE_FAMILY_PTR, CURRENT) \
{ \
    for(VM_PAGE_FAMILY_PTR; VM_PAGE_FAMILY_PTR->first_page; CURRENT = CURRENT->pg_family){ \

#define ITER_VM_PAGE_END(VM_PAGE_FAMILY_PTR, CURRENT) }} \

/*CURRENT é um ponteiro para um meta block*/
#define ITER_VM_PAGE_ALL_BLOCKS_BEGIN(VM_PAGE_PTR, CURRENT) \
{   \
    meta_block_data_t* next = NULL;\
    CURRENT = &VM_PAGE_PTR->meta_block_data; \
    for(CURRENT; CURRENT; CURRENT = NEXT_META_BLOCK(CURRENT)){\
        printf("Meta Block Size: [%d]\nOffset: [%d]\nPrev Block Addr: [%p]\nNext Block Addr: [%p]\n", \
                CURRENT->block_size, CURRENT->offset, CURRENT->prev_block, CURRENT->next_block); \
    
#define ITER_VM_PAGE_ALL_BLOCKS_BEGIN(VM_PAGE_PTR, CURRENT) }} \

#define OFFSET_OF(CONTAINER_STRUCT, FIELD_NAME) \
    (CONTAINER_STRUCT, #FIELD_NAME)(size_t)(&((CONTAINER_STRUCT *)0)->FIELD_NAME)

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
    
#define SET_VM_PAGE_EMPTY(VM_PAGE_T_PTR) \
    VM_PAGE_T_PTR->meta_block_data.is_free == MMANAGER_TRUE \
    VM_PAGE_T_PTR->meta_block_data.prev_block = NULL \
    VM_PAGE_T_PTR->meta_block_data.next_block = NULL

#define MAXSIZE_PAGE_FAMILY_NAME 32 


struct vm_page_family_t{
    char struct_name[MAXSIZE_PAGE_FAMILY_NAME];
    uint32_t size;
    vm_page_t *first_page;
};

struct vm_page_for_families_t{
    struct vm_page_for_families_* next;
    vm_page_family_t vm_page_family[0];
    uint32_t vm_page_for_families_quantity;
};

struct vm_meta_block_data_t{
    vm_bool_t is_free;
    uint32_t block_size;
    struct vm_meta_block_data_t *prev_block;
    struct vm_meta_block_data_t *next_block;
    uint32_t offset;
};

/*Estrutura para navegação entre as vm pages. Ou seja, quando o heapmanager receber uma nova
  página de memória virtual do kernel, será feito um cast para o tipo dessa struct(vm_page_t).
 *pg_family aponta para a vm_page_family_t anterior.
  char page_memory[0] é o primeiro bloco de dados (data block) na vm page.  
 */
struct vm_page_t{
    vm_page_t *prev;
    vm_page_t *next;
    vm_page_family_t* pg_family;
    vm_meta_block_data_t meta_block_data;
    char page_memory[0];
};

void mmanager_init(void);
void mmanager_new_page_family(char* stuct_name, uint32_t size);
void mmanager_print_registered_page_families(void);
void mmanager_print_meta_blocks_vm_page(vm_meta_block_data_t* meta_block);
static void* mmanager_page_alloc(int vmp_units);
vm_bool_t mmanager_is_vm_page_empty(vm_page_t* vm_page);
static void* mmanager_merge_free_blocks(vm_meta_block_data_t* first_block, vm_meta_block_data_t* second_block);
static signed int mmanager_page_dealloc(void* memory_page_addr, int units);
vm_page_family_t* lookup_page_family_by_name(char *struct_name);

#endif