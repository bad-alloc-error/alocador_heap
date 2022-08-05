#ifndef _MMANAGER_API_H_
#define _MMANAGER_API_H_
#include "headers/mmanager.h"
/*Interface entre o Heap Manager Lib e aplicação do usuário*/

void mmanager_init();

void mmanager_new_page_family(char* struct_name, uint32_t struct_site);

#define MMANAGER_STRUCT_REGISTER(struct_name) \
    (mmanager_new_page_family(#struct_name, sizeof(struct_name)))


#endif