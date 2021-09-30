#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#ifdef NODEBUG
#define log_buf(fmt, ...) (void(0))
#else
#define log_buf(fmt, ...)                                                     \
        do {                                                                  \
                fprintf(stderr, fmt, ##__VA_ARGS__);                          \
        } while (0)
#endif

inline void log_flush()
{
        fflush(stderr);
}

#ifdef NODEBUG
#define log(fmt, ...) (void(0))
#else
#define log(fmt, ...)                                                           \
        do {                                                                    \
                fprintf(stderr, "[%s] in %s at %s(%d): " fmt,                   \
                        __TIME__, __FILE__, __func__, __LINE__, ##__VA_ARGS__); \
                fflush(stderr);                                                 \
        } while (0)
#endif


#endif /* LOG_H */

