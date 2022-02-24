#define LOG_BUFFER_SIZE 512

#define ANSI_PREFIX "\033"
#define ANSI_RESET "[39m\n"

#define COLOR_ERROR     "[31m"
#define COLOR_INFO      "[32m"
#define COLOR_DEBUG     "[33m"

#define DEBUG_LEVEL_INFO "info"
#define DEBUG_LEVEL_ERROR "error"
#define DEBUG_LEVEL_DEBUG "debug"

void log_error(const char* message, ...);
void log_info(const char* message, ...);
void log_debug(const char* message, ...);