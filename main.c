#include "headers/mmanager_api.h"
#include<stdio.h>

int main(){

    /*TESTANDO*/
    mmanager_init();

    typedef struct estudante{
        int valor1;
        int valor2;

    }estudante_t;

    estudante_t estudante1;
    estudante_t estudante2;
    estudante_t estudante3;
    estudante1.valor1 = 19;
    estudante1.valor2 = 88;
    estudante2.valor1 = 55;
    estudante3.valor2 = 30;

    MMANAGER_STRUCT_REGISTER(estudante1);
    MMANAGER_STRUCT_REGISTER(estudante2);
    MMANAGER_STRUCT_REGISTER(estudante3);

    mmanager_print_registered_page_families();
    vm_page_family_t* p = lookup_page_family_by_name("estudante3");

    printf("name: %s\n", p->struct_name);
    printf("size: %d\n", p->size);
    

    return 0;
}