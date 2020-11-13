#ifndef USEFUL_LIB_MISC_STRING
#define USEFUL_LIB_MISC_STRING

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

int _strcmpi(const char* a, const char* b);
void _strtrim(char* trim, char const *str);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif