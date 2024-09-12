
#pragma once



#define CONSOLE_COLOR_RESET "\033[0m"
#define CONSOLE_COLOR_GREEN "\033[1;32m"
#define CONSOLE_COLOR_RED "\033[1;31m"
#define CONSOLE_COLOR_PURPLE "\033[1;35m"
#define CONSOLE_COLOR_CYAN "\033[0;36m"
#define CONSOLE_COLOR_YELLOW "\033[1;33m"
#define CONSOLE_COLOR_BLUE "\033[0;34m"

void Log(int severity, const char* fmt, ...);
void Log(int severity, const char* fmt, va_list args);

#define INFO(fmt, ...) Log(0, fmt, ##__VA_ARGS__)
#define WARNING(fmt, ...) Log(1, fmt, ##__VA_ARGS__)
#define ERROR(fmt, ...) Log(2, fmt, ##__VA_ARGS__)
#define PRINT(fmt, ...) Log(3, fmt, ##__VA_ARGS__)

char *LoadTextFile(const char *fileName);
void FreeTextFile(char *text);


static inline bool matchString(const char *str1, const char *str2,size_t bLen)
{
    size_t aLen = strlen(str1);
    
    if (aLen != bLen) return false;
    
    return (memcmp(str1, str2, aLen) == 0);
    
}

static inline size_t  GROW_CAPACITY(size_t capacity)
{
    return ((capacity) < 8 ? 8 : (capacity) * 2);
}

static inline double time_now()
{
    return (clock() / (double)CLOCKS_PER_SEC);
}

const char *FormatText(const char *text, ...);

class FatalException : public std::runtime_error
{
public:
    explicit FatalException(std::string message) : std::runtime_error(message) {}
};


