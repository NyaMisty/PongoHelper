#ifdef DEBUG_STUB_CALL
#define stub_log printf
#else
void stub_log(const char *a, ...){
    return;
}
#endif

int putchar(int c) {
    printf32("%c", c);
    return c;
}

int isprint(int c){
    return c >= 32 && c < 127;
}

int __tolower(int c) {
    if (c >= 'A' && c <= 'Z') {
        c -= 'A' - 'a';
    }
    return c;
}

#undef memcpy
void *my_memcpy(void *dst, const void *src, size_t size);
extern void *memcpy(void *dst, const void *src, size_t size) {
    return my_memcpy(dst, src, size);
}
#define memcpy my_memcpy
extern void *my_memcpy(void *dst, const void *src, size_t size) {
    for (size_t i = 0; i < size; i++) {
        ((char *)dst)[i] = ((char *)src)[i];
    }
    return dst;
}

#undef bzero
extern void bzero(void *s, size_t n){
    memset(s, 0, n);
}

#undef strcmp
int my_strcmp(const char *str1, const char *str2);
extern int strcmp(const char *str1, const char *str2) {
    return my_strcmp(str1, str2);
}
#define strcmp my_strcmp
extern int my_strcmp(const char *str1, const char *str2)
{
    size_t i;

    for(i = 0; str1[i] == str2[i] && str1[i]  && str2[i] ; ++i)
        ;
    return str1[i] - str2[i];
}

#undef strlen
size_t my_strlen(const char *str);
extern size_t strlen(const char *str) {
    return my_strlen(str);
}
#define strlen my_strlen
extern size_t my_strlen(const char *str)
{
    size_t i;

    for(i = 0; str[i] ; ++i)
        ;
    return i;
}

#undef strcpy
char *my_strcpy(char *dest, const char *src);
extern char *strcpy(char *dest, const char *src) {
    return  my_strcpy(dest, src);
}
#define strcpy my_strcpy
extern char *my_strcpy(char *dest, const char *src)
{
    size_t i;

    for(i = 0; src[i] ; ++i)
        dest[i] = src[i];
    dest[i] = '\0';
    return dest;
}


#undef strncmp
int my_strncmp( const char * str1, const char * str2, size_t num );
extern int strncmp( const char * str1, const char * str2, size_t num ) {
    return my_strncmp(str1, str2, num);
}
#define strncmp my_strncmp
extern int my_strncmp( const char * str1, const char * str2, size_t num ){
    stub_log("strncmp called\n");
    while (1) {
        char c1 = *str1++;
        char c2 = *str2++;
        if (c1 != c2) {
            if (c1 < c2){
                stub_log("strncmp called\n");
                return -1;
            }
            if (c1 > c2) {
                stub_log("strncmp called\n");
                return 1;
            }
        }
        if (c1 == 0)
            break;
    }
    stub_log("strncmp end\n");
    return 0;
}

#undef strncpy
char* my_strncpy(char* destination, const char* source, size_t num);
extern char* strncpy(char* destination, const char* source, size_t num) {
    return my_strncpy(destination, source, num);
}
#define strncpy my_strncpy
extern char* my_strncpy(char* destination, const char* source, size_t num)
{
    stub_log("strncpy s\n");
    // return if no memory is allocated to the destination
    if (destination == NULL)
        return NULL;

    // take a pointer pointing to the beginning of destination string
    char* ptr = destination;

    // copy first num characters of C-string pointed by source
    // into the array pointed by destination
    while (*source && num--)
    {
        *destination = *source;
        destination++;
        source++;
    }

    // null terminate destination string
    *destination = '\0';

    // destination is returned by standard strncpy()
    stub_log("strncpy e\n");
    return ptr;
}

#undef strrchr
char *my_strrchr(const char *str, int c);
extern char *strrchr(const char *str, int c) {
    return my_strrchr(str, c);
}
#define strrchr my_strrchr
extern char *my_strrchr(const char *str, int c)
{
    stub_log("strncmp s\n");
    char *p = NULL;

    for(; *str ; ++str)
    {
        if(*str == c)
            p = (char*) str;
    }
    stub_log("strncmp end\n");
    return p;
}

#undef strstr
char *my_strstr(const char *haystack, const char *needle);
extern char *strstr(const char *haystack, const char *needle) {
    return my_strstr(haystack, needle);
}
#define strstr my_strstr
extern char *my_strstr(const char *haystack, const char *needle)
{
    stub_log("strstr s\n");
    size_t n = strlen(needle);
    for(; *haystack ; ++haystack)
    {
        if(strncmp(haystack, needle, n) == 0){
            stub_log("strstr e\n");
            return (char *) haystack;
        }
    }
    stub_log("strstr e\n");
    return 0;
}

#undef memset
void *my_memset(void *dst, int val, size_t size);
extern void *memset(void *dst, int val, size_t size) {
    return my_memset(dst, val, size);
}
#define memset my_memset
extern void *my_memset(void *dst, int val, size_t size) {
    stub_log("memset s\n");
    stub_log("size: %d\n", size);
    int i = 0;
    while (i < size){
        if (size - i >= 4) {
            ((uint32_t *)dst)[i] = val;
            i += 4;
        } else if (size - i >= 2) {
            ((uint16_t *)dst)[i] = val;
            i += 2;
        } else {
            ((uint8_t *)dst)[i] = val;
            i += 1;
        }
        /*if (!(i % 128)) {
            stub_log("cur: %d\n", i);
        }*/
    }
    stub_log("memset e\n");
    return dst;
}


#undef memmove
void *my_memmove(void *pvDst, const void *pvSrc, size_t n);
extern void *memmove(void *pvDst, const void *pvSrc, size_t n) {
    return my_memmove(pvDst, pvSrc, n);
}
#define memmove my_memmove
extern void *my_memmove(void *pvDst, const void *pvSrc, size_t n) {
    stub_log("memmove s\n");
    unsigned char u8CopyByte=1;  //flag bit
    char *pcSource =(char *)pvSrc;
    char *pcDstn =(char *)pvDst;

    if(pcSource == pcDstn) { //Source & destination have same address
        u8CopyByte=0;
    } else if(pcSource < pcDstn && pcDstn < pcSource + n) { // overlap
     /* If overlap then copy from end*/
        pcSource += n;
        pcDstn += n;
   }
    if(u8CopyByte) {
        while(n--)
        *--pcDstn = *--pcSource;
    }
 
    stub_log("memmove s\n");
    return pvDst;
}
