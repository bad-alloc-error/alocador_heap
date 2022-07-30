#ifndef _MMANAGER_H_
#define _MMANAGER_H_

void mmanager_init();
static void* mmanager_page_alloc(int vmp_units);
static signed int mmanager_page_dealloc(void* memory_page_addr, int units);

#endif