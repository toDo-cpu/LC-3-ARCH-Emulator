#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include "log.h"


void __log_format(char * tag, char *color, const char* message, va_list args) {
    time_t now; 
    time(&now);
    char * date =   ctime(&now);
    date[strlen(date) - 1] = '\0';
    printf("%s%s%s (%s) ",ANSI_PREFIX ,color, date, tag);
    vprintf(message, args);
    printf(ANSI_RESET); 
}
void log_error(const char* message, ...) {
    va_list args;
    va_start(args, message);
    __log_format(DEBUG_LEVEL_ERROR, COLOR_ERROR, message, args);
    va_end(args); 
}

void log_info(const char* message, ...) {
   va_list args;
   va_start(args, message);
   __log_format(DEBUG_LEVEL_INFO, COLOR_INFO, message, args);
   va_end(args); 
}

void log_debug(const char* message, ...) {   
    va_list args;   
    va_start(args, message);
    __log_format(DEBUG_LEVEL_DEBUG, COLOR_DEBUG, message, args);
    va_end(args); 
}