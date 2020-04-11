#include <string.h>
#ifndef __APPLE__
#define DEF_WEAK(a)
#define __BIONIC_ALIGN(__value, __alignment) (((__value) + (__alignment)-1) & ~((__alignment)-1))
#define	__predict_true(exp)	__builtin_expect((exp) != 0, 1)
#define	__predict_false(exp)	__builtin_expect((exp) != 0, 0)
// #define gettid() syscall(SYS_gettid)
extern
#ifdef __cplusplus
"C"
#endif
size_t strlcpy(char *dst, const char *src, size_t dsize);
#else
#define gettid() 0
#define __assert my_assert
extern 
#ifdef __cplusplus
"C"
#endif
void my_assert(const char* file, int line, const char* msg);
/* Used to retry syscalls that can return EINTR. */
#define TEMP_FAILURE_RETRY(exp) ({         \
    __typeof__(exp) _rc;                   \
    do {                                   \
        _rc = (exp);                       \
    } while (_rc == -1 && errno == EINTR); \
    _rc; })

#include <stdio.h>
#include <sys/types.h>
#include "../core/base/include/android-base/off64_t.h"
#  define fopen64 fopen
#  define mmap64 mmap
#  define pread64 pread
#define PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP PTHREAD_RECURSIVE_MUTEX_INITIALIZER
#include <libgen.h>
#define basename(name) basename((char*)name)
#endif