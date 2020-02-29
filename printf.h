#include "printf/printf.h"
/*#define _vsnprintf vsnprintf
#undef vsnprintf
extern int vsnprintf (char * s, size_t n, const char * format, va_list arg ) {
    return _vsnprintf(s, n, format, arg);
}*/

// for our printf
void _putchar(char c) {
    printf32("%c", c);
}
