#ifndef _MMANAGER_API_H_
#define _MMANAGER_API_H_

#include "mmanager.h"
/*Interface entre o Heap Manager Lib e aplicação do usuário*/

void mmanager_init(void);
vm_page_family_t* lookup_page_family_by_name (char *struct_name);
void mmanager_new_page_family(char* struct_name, uint32_t struct_size);
void mmanager_print_registered_page_families();

#define MMANAGER_STRUCT_REGISTER(STRUCT_NAME) \
    (mmanager_new_page_family(#STRUCT_NAME, sizeof(STRUCT_NAME)))


#endif