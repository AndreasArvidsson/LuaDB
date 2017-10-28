#include "Log.h"

#ifdef DEBUG

#include <cstdarg>
#include <stdio.h>

#define BUFFER_SIZE 512

void Log::log(const char*const type, const char*const file, const unsigned int line, const char *const str, ...) {
    va_list ap;
    char text[BUFFER_SIZE];
    va_start(ap, str);
    vsnprintf(text, BUFFER_SIZE, str, ap);
    va_end(ap);
	printf("%s: %s(%d): %s\n", type, file, line, text);
}

#endif