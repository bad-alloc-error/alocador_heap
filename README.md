# heapmanager
Alocador de Memória

<b>Objetivo:</b>
  Gerenciador de alocação de memória para aplicações user space. Para alocação utiliza o método worst fit e gerencia os chunks livres de memória através de uma fila de prioridades ou futuramente utilizando max-heap. A principio a primeira implementação será com fila de prioridades. Esse gerenciador evita a fragmentação da heap alocando grandes páginas de memória virtual chamadas de vm_page_t. E elas serão colocadas juntas em uma estrutura chamada de vm_page_for_families, ou sejá, são "familias" de páginas virtuais. Atualizarei em breve esse readme explicando a arquitetura dessa aplicação, as fases de implementação, o que já foi feito e o que falta fazer. 
