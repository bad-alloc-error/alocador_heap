#ifndef _MMANAGER_H_
#define _MMANAGER_H_

/*Para obter o número máximo de page_family_t que podemos armazenar dentro de uma page
    do tipo vm_page_for_families_t é pegar o tamanho da pagina alocada, subtrair pelo tamanho
    do ponteiro do tipo vm_page_for_families_t e dividir pelo tamanho da objeto vm_page_family_t.
    Na base da página (vm_page_for_families_t) tera um campo onde armazeno o endereço 
    da proxima vm_page_for_families_t e a partir dai as vm_page_family_t serão empilhadas no espaço
    remanescente da página. */

#define MAX_FAMILIES_PER_VM_PAGE \
    (PAGE_SIZE - sizeof(vm_page_for_families_t *)/\
        sizeof(vm_page_family_t))

#define ITER_PAGE_FAMILY_BEGIN(first_page_for_families_ptr, current) \
{                                                          \
    __uint32_t count_macro = 0;                                  \
    for(current = (vm_page_family_t *)&first_page_for_families_ptr->vm_page_family[0]; \
                    current->size && count < MAX_FAMILIES_PER_VM_PAGE; current++, count_macro++){ \

#define ITER_PAGE_FAMILY_END(first_page_for_families_ptr, current) }}

#define MAXSIZE_PAGE_FAMILY_NAME 32

typedef struct vm_page_family_{
    char struct_name[MAXSIZE_PAGE_FAMILY_NAME];
    __uint32_t size;
}vm_page_family_t;

typedef struct vm_page_for_families_{
    struct vm_page_for_families_t *next;
    vm_page_family_t vm_page_family[0];
}vm_page_for_families_t;


void mmanager_init(void);
static void* mmanager_page_alloc(int vmp_units);
static signed int mmanager_page_dealloc(void* memory_page_addr, int units);
void mmanager_new_page_family(char* stuct_name, __uint32_t size);

#endif