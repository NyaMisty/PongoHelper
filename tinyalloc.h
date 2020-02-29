#ifdef USE_TA
//#define TA_DEBUG
#include "tinyalloc/tinyalloc.h"
#ifdef TA_DEBUG
void print_s(char *input) {
    command_puts(input);
}
void print_i(size_t input) {
    printf("%d", input);
}
#endif
void init_ta() {
    void *malloc_test = malloc(0x8000);
    ((uint64_t *)malloc_test)[0] = 0;
    print_addr("malloc_test", malloc_test);
    memset(malloc_test, 0, 0x8000);
    //uint64_t heapstart = loader_xfer_recv_data + 0x100000;
    //uint64_t heapend = heapstart + 0x80000;
    //memset(heapstart, 0, heapend - heapstart);
    printf("ta_init: %d\n", ta_init(malloc_test, malloc_test + 0x8000, 128, 16, 8));
    printf("ta_check: %d\n", ta_check());
    void *test = ta_alloc(10);
    print_addr("ta_alloc(10)", test);
    test = ta_calloc(1, 200);
    print_addr("ta_calloc(200)", test);
    printf("ta_check: %d\n", ta_check());
}
#else
void init_ta(){
    
}
#endif
