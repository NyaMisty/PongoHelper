#include "capstone/capstone.h"
#ifdef USE_TA
void* my_cs_malloc(size_t size) {
    stub_log("malloc %d\n", size);
    void *ret = ta_alloc(size);
    stub_log("malloc end");
    return ret;
}

void* my_cs_calloc(size_t nmemb, size_t size) {
    stub_log("calloc: %x\n", nmemb * size);
    void *ret = ta_calloc(nmemb, size);
    stub_log("calloc_ret: %llx\n", ret);
    stub_log("calloc end\n");
    return ret;
}

void* my_cs_realloc(void *ptr, size_t size){
    stub_log("realloc %d\n", size);
    void *new = ta_alloc(size);
    memcpy(new, ptr, size);
    ta_free(ptr);
    stub_log("realloc end\n");
    return new;
}

void my_cs_free(void *ptr) {
    stub_log("free\n");
    //ta_free(ptr);
    //free(ptr);
}

int my_cs_vsnprintf(char *str, size_t size, const char *format, va_list ap){
    stub_log("%s", format);
    return vsnprintf(str, size, format, ap);
}
#else
void* my_cs_malloc(size_t size) {
    stub_log("malloc %d\n", size);
    //void *ret = ta_alloc(size);
    void *ret = malloc(size);
    stub_log("malloc end\n");
    return ret;
}

void* my_cs_calloc(size_t nmemb, size_t size) {
    stub_log("calloc %d\n", nmemb * size);
    void *ret = malloc(nmemb * size);
    memset(ret, 0, nmemb * size);
    //void *ret = ta_calloc(nmemb, size);
    stub_log("calloc_ret: %llx\n", ret);
    stub_log("calloc end\n");
    return ret;
}

void* my_cs_realloc(void *ptr, size_t size){
    stub_log("realloc %d\n", size);
    //void *new = ta_alloc(size);
    void *new = malloc(size);
    memcpy(new, ptr, size);
    free(ptr);
    //ta_free(ptr);
    stub_log("realloc end\n");
    return new;
}

void my_cs_free(void *ptr) {
    stub_log("free\n");
    //ta_free(ptr);
    //free(ptr);
}

int my_cs_vsnprintf(char *str, size_t size, const char *format, va_list ap){
    stub_log("%s", format);
    return vsnprintf(str, size, format, ap);
}
#endif
#undef vsnprintf
cs_opt_mem cs_mem_setup = {
    .malloc = my_cs_malloc,
    .calloc = my_cs_calloc,
    .realloc = my_cs_realloc,
    .free = my_cs_free,
    .vsnprintf_ = my_cs_vsnprintf,
};

csh cs_handle;

void setup_capstone() {
    printf("Set up cs_opt_mem\n", 0);
    cs_option(0, CS_OPT_MEM, &cs_mem_setup);
    
    printf("Starting to cs_open\n", 0);
    
    int ret = cs_open(CS_ARCH_ARM64, CS_MODE_ARM, &cs_handle);
    if (ret) {
        printf("ERROR: Failed to initialize engine!, err=%d\n", ret);
        return;
    }
    printf("Successfully initialized capstone engine\n", 0);
    cs_option(cs_handle, CS_OPT_SKIPDATA, CS_OPT_ON);
    printf("Set up skipdata\n", 0);
}

