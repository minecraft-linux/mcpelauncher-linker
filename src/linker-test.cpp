typedef long long off64_t;

#include <fcntl.h>
#include <mcpelauncher/linker.h>

__attribute__((sysv_abi)) static void __abort() {
    abort();
}

// template<const char* msg> struct aborthelper {

//     static void __myabort() {
//         printf("%s\n", msg);
//         abort();
//     }
// };

template<auto symbols, size_t i> struct aborthelper {

    static void __myabort() {
        printf("%s\n", symbols[i]);
        // abort();
    }
};

__attribute__((sysv_abi)) void print(const char* message) {
    printf("%s\n", message);
}

#include <io.h>
#include <locale.h>
#include <wchar.h>
#include <uchar.h>
#include <vector>
#include <sstream>
#include <shared_mutex>
#include <atomic>
#include <mutex>
#include <chrono>
#include <memory>
#include <condition_variable>


struct pthread_mutexattr_t {
    int type = 0;
};

__attribute__((sysv_abi)) int pthread_mutexattr_init(pthread_mutexattr_t *attr) {
    new (attr) pthread_mutexattr_t();
    return 0;
}

__attribute__((sysv_abi)) int pthread_mutexattr_destroy(pthread_mutexattr_t *attr) {
    attr->~pthread_mutexattr_t();
    return 0;
}

__attribute__((sysv_abi)) int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type) {
    attr->type = type;
    return 0;
}

struct pthread_mutex_t {
    std::atomic_int32_t initialized = 0xffffffff;
    std::shared_ptr<std::recursive_mutex> mtx = std::make_shared<std::recursive_mutex>();
};

__attribute__((sysv_abi)) int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t * attr) {
    new (mutex) pthread_mutex_t();
    return 0;
}

__attribute__((sysv_abi)) int pthread_mutex_lock(pthread_mutex_t *mutex) {
    if(!mutex->initialized) {
        new (mutex) pthread_mutex_t();
    }
    mutex->mtx->lock();
    return 0;
}
__attribute__((sysv_abi)) int pthread_mutex_trylock(pthread_mutex_t *mutex) {
    if(!mutex->initialized) {
        new (mutex) pthread_mutex_t();
    }
    return mutex->mtx->try_lock() ? 0 : 1;

}
__attribute__((sysv_abi)) int pthread_mutex_unlock(pthread_mutex_t *mutex) {
    mutex->mtx->unlock();
    return 0;
}

__attribute__((sysv_abi)) int pthread_mutex_destroy(pthread_mutex_t *mutex) {
    mutex->~pthread_mutex_t();
    return 0;
}


struct bionic_timespec {
  /** Number of seconds. */
  time_t tv_sec;
  /** Number of nanoseconds. Must be less than 1,000,000,000. */
  long long tv_nsec;
};

typedef int clockid_t;

struct pthread_condattr_t {
    clockid_t clock = 0;
    int pshared = 0;
};

struct pthread_cond_t {
    std::atomic_int32_t initialized = 0xffffffff;
    std::shared_ptr<std::condition_variable_any> condvar = std::make_shared<std::condition_variable_any>();
};

__attribute__((sysv_abi)) int pthread_condattr_init(pthread_condattr_t *attr) {
    new (attr) pthread_condattr_t();
    return 0;
}
__attribute__((sysv_abi)) int pthread_condattr_destroy(pthread_condattr_t *attr) {
    attr->~pthread_condattr_t();
    return 0;
}
__attribute__((sysv_abi)) int pthread_condattr_getpshared(const pthread_condattr_t *attr, int* pshared) {
    *pshared = attr->pshared;
    return 0;
}
__attribute__((sysv_abi)) int pthread_condattr_setpshared(pthread_condattr_t *attr, int pshared) {
    attr->pshared = pshared;
    return 0;
}
__attribute__((sysv_abi)) int pthread_condattr_getclock(const pthread_condattr_t *attr, clockid_t * clock) {
    *clock = attr->clock;
    return 0;
}
__attribute__((sysv_abi)) int pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t clock) {
    attr->clock = clock;
    return 0;
}

__attribute__((sysv_abi)) int pthread_cond_init(pthread_cond_t * cond, const pthread_condattr_t * attr) {
    // printf("pthread_cond_init 0x%llx\n", (long long)cond);
    new (cond) pthread_cond_t();
    return 0;
}
__attribute__((sysv_abi)) int pthread_cond_destroy(pthread_cond_t *cond) {
    // printf("pthread_cond_destroy 0x%llx\n", (long long)cond);
    cond->~pthread_cond_t();
    return 0;
}

__attribute__((sysv_abi)) int pthread_cond_broadcast(pthread_cond_t *cond) {
    // printf("pthread_cond_broadcast 0x%llx\n", (long long)cond);
    if(!cond->initialized) {
        new (cond) pthread_cond_t();
    }
    cond->condvar->notify_all();
    return 0;
}

__attribute__((sysv_abi)) int pthread_cond_signal(pthread_cond_t *cond) {
    // printf("pthread_cond_signal 0x%llx\n", (long long)cond);
    if(!cond->initialized) {
        new (cond) pthread_cond_t();
    }
    cond->condvar->notify_one();
    return 0;
}

__attribute__((sysv_abi)) int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct bionic_timespec *abs_timeout) {
    // printf("pthread_cond_timedwait 0x%llx\n", (long long)cond);
    if(!cond->initialized) {
        new (cond) pthread_cond_t();
    }
    // std::unique_lock<std::recursive_mutex> lk(*mutex->mtx);
    std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds> tp;
    tp += std::chrono::seconds(abs_timeout->tv_sec);
    tp += std::chrono::nanoseconds(abs_timeout->tv_nsec);
    printf("tp - std::chrono::steady_clock::now(): %lld\n", (tp - std::chrono::steady_clock::now()).count());
    cond->condvar->wait_until(*mutex->mtx, tp);
    return 0;
}
__attribute__((sysv_abi)) int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) {
    // printf("pthread_cond_wait 0x%llx\n", (long long)cond);
    if(!cond->initialized) {
        new (cond) pthread_cond_t();
    }
    // std::unique_lock<std::recursive_mutex> lk(*mutex->mtx);
    cond->condvar->wait(*mutex->mtx);
    return 0;
}

//  sht;
struct pthread_rwlock_t {
    std::atomic_int32_t initialized = 0xffffffff;
    std::shared_timed_mutex shared;
    std::atomic_bool release_writer; 
};
struct pthread_rwlockattr_t{
    
};
__attribute__((sysv_abi)) int pthread_rwlock_init(pthread_rwlock_t* rwlock_interface, const pthread_rwlockattr_t* attr) {
    new (rwlock_interface) pthread_rwlock_t();
    return 0;
}

__attribute__((sysv_abi)) int clock_gettime(clockid_t clock_id, struct bionic_timespec *tp) {
    auto now = std::chrono::steady_clock::now();
    auto nanosecs = now.time_since_epoch().count();
    tp->tv_nsec = nanosecs % 1000000000;
    tp->tv_sec = nanosecs / 1000000000;
    return 0;
}

struct bionic_timeval {
    time_t      tv_sec;     /* seconds */
    long long tv_usec;    /* microseconds */
};
__attribute__((sysv_abi)) int gettimeofday(struct bionic_timeval * tv, struct timezone * tz) {
    auto now = std::chrono::steady_clock::now();
    auto nanosecs = now.time_since_epoch().count();
    tv->tv_usec = (nanosecs % 1000000000) / 1000;
    tv->tv_sec = nanosecs / 1000000000;
    return 0;
}


#include <functional>
struct defer {
  std::function<void()> func;
  ~defer() {
    func();
  }
};

__attribute__((sysv_abi)) int pthread_rwlock_timedwrlock(pthread_rwlock_t* rwlock_interface, const bionic_timespec* abs_timeout) {
    if(!rwlock_interface->initialized) {
        new (rwlock_interface) pthread_rwlock_t();
    }
    std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds> tp;
    tp += std::chrono::seconds(abs_timeout->tv_sec);
    tp += std::chrono::nanoseconds(abs_timeout->tv_nsec);
    int ret = rwlock_interface->shared.try_lock_until(tp) ? 0 : ETIMEDOUT;
    if(!ret) {
        rwlock_interface->release_writer = true;
    }
    return ret;
}

__attribute__((sysv_abi)) int pthread_rwlock_wrlock(pthread_rwlock_t* rwlock_interface) {
    if(!rwlock_interface->initialized) {
        new (rwlock_interface) pthread_rwlock_t();
    }
    rwlock_interface->shared.lock();
    rwlock_interface->release_writer = true;
    return 0;
}

__attribute__((sysv_abi)) int pthread_rwlock_trywrlock(pthread_rwlock_t* rwlock_interface) {
    if(!rwlock_interface->initialized) {
        new (rwlock_interface) pthread_rwlock_t();
    }
    int ret = rwlock_interface->shared.try_lock() ? 0 : EBUSY;
    if(!ret) {
        rwlock_interface->release_writer = true;
    }
    return ret;
}

// __attribute__((sysv_abi)) int pthread_rwlock_wrunlock(pthread_rwlock_t* rwlock_interface) {
//     if(!rwlock_interface->initialized) {
//         new (rwlock_interface) pthread_rwlock_t();
//     }
//     rwlock_interface->shared.unlock();
//     return 0;
// }

__attribute__((sysv_abi)) int pthread_rwlock_rdlock(pthread_rwlock_t* rwlock_interface) {
    if(!rwlock_interface->initialized) {
        new (rwlock_interface) pthread_rwlock_t();
    }
    rwlock_interface->shared.lock_shared();
    rwlock_interface->release_writer = false;
    return 0;
}

__attribute__((sysv_abi)) int pthread_rwlock_tryrdlock(pthread_rwlock_t* rwlock_interface) {
    if(!rwlock_interface->initialized) {
        new (rwlock_interface) pthread_rwlock_t();
    }
    int ret = rwlock_interface->shared.try_lock_shared() ? 0 : EBUSY;
    if(!ret) {
        rwlock_interface->release_writer = false;
    }
    return ret;
}

// __attribute__((sysv_abi)) int pthread_rwlock_rdunlock(pthread_rwlock_t* rwlock_interface) {
//     if(!rwlock_interface->initialized) {
//         new (rwlock_interface) pthread_rwlock_t();
//     }
//     rwlock_interface->shared.unlock_shared();
//     return 0;
// }

__attribute__((sysv_abi)) int pthread_rwlock_unlock(pthread_rwlock_t* rwlock_interface) {
    if(!rwlock_interface->initialized) {
        new (rwlock_interface) pthread_rwlock_t();
    }
    if(rwlock_interface->release_writer) {
        rwlock_interface->shared.unlock();
    } else {
        rwlock_interface->shared.unlock_shared();
    }
    return 0;
}

__attribute__((sysv_abi)) int pthread_rwlock_destroy(pthread_rwlock_t* rwlock_interface) {
    rwlock_interface->~pthread_rwlock_t();
    return 0;
}

#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <windows.h>
#include <processthreadsapi.h>

struct pthread_key_t {
    DWORD keyid;
    // void (*destructor)(void*);
};
__attribute__((sysv_abi)) int pthread_key_create(pthread_key_t *key, void (*destructor)(void*)) {
    new (key) pthread_key_t();
    key->keyid = TlsAlloc();
    // key->destructor = destructor;
    return 0;
}

__attribute__((sysv_abi)) void *pthread_getspecific(pthread_key_t key) {
    return TlsGetValue(key.keyid);
}
__attribute__((sysv_abi)) int pthread_setspecific(pthread_key_t key, const void *value) {
    // if(key->destructor) {
    //     key->destructor(TlsGetValue(key->keyid));
    // }
    TlsSetValue(key.keyid, (void *)value);
    return 0;
}

#include <iostream>
#include <exception>
// std::atomic oncecnt;
// std::mutex pthread_once_mtx;
std::unordered_map<int*, std::unique_ptr<std::once_flag>> pthread_once_flags;
__attribute__((sysv_abi)) int pthread_once(int *once_control, void (__attribute__((sysv_abi)) *init_routine)(void)) {
    // std::once_flag* flag;
    // {
    //     std::lock_guard<std::mutex> guard(pthread_once_mtx);
    //     try {
    //         flag = pthread_once_flags[once_control].get();
    //         if(!flag) {
    //             flag = (pthread_once_flags[once_control] = std::make_unique<std::once_flag>()).get();
    //         }
    //     } catch (const std::bad_alloc& e)
    //     {
    //         std::cout << "Allocation failed: " << e.what() << '\n';
    //         return 1;
    //     }
    // }
    // std::call_once(*flag, [init_routine]() {
    //     init_routine();
    // });
    // if constexpr(sizeof(std::atomic_int) != sizeof(LONG)) {
    //     abort();
    // }
    LONG volatile *lock = (LONG *)once_control;
    LONG result;

    if (*lock == 1)
        return 1;

    do {
        result = InterlockedCompareExchange(lock, 2, 0);
        if (result == 0) {
            init_routine();
            *lock = 1;
            return 1;
        }
    } while (result == 2);
    return 0;
}

typedef HANDLE pthread_t;

struct pthread_attr_t {
    int dummy = 0;
};

__attribute__((sysv_abi)) int pthread_attr_destroy(pthread_attr_t *attr) {
    attr->~pthread_attr_t();
    return 0;
}
__attribute__((sysv_abi)) int pthread_attr_getdetachstate(const pthread_attr_t *, int *);
__attribute__((sysv_abi)) int pthread_attr_getguardsize(const pthread_attr_t *,
                     size_t *);
__attribute__((sysv_abi)) int pthread_attr_getinheritsched(const pthread_attr_t *,
                     int *);
__attribute__((sysv_abi)) int pthread_attr_getschedparam(const pthread_attr_t *,
                     struct sched_param *);
__attribute__((sysv_abi)) int pthread_attr_getschedpolicy(const pthread_attr_t *,
                     int *);
__attribute__((sysv_abi)) int pthread_attr_getscope(const pthread_attr_t *,
                     int *);
__attribute__((sysv_abi)) int pthread_attr_getstack(const pthread_attr_t *,
                     void **, size_t *);
__attribute__((sysv_abi)) int pthread_attr_getstacksize(const pthread_attr_t *,
                     size_t *);
__attribute__((sysv_abi)) int pthread_attr_init(pthread_attr_t *attr) {
    new (attr) pthread_attr_t();
    return 0;
}
__attribute__((sysv_abi)) int pthread_attr_setdetachstate(pthread_attr_t *attr, int detached) {
    return 0;
}
__attribute__((sysv_abi)) int pthread_attr_setguardsize(pthread_attr_t *, size_t);
__attribute__((sysv_abi)) int pthread_attr_setinheritsched(pthread_attr_t *, int);
__attribute__((sysv_abi)) int pthread_attr_setschedparam(pthread_attr_t *,
                     const struct sched_param *);
__attribute__((sysv_abi)) int pthread_attr_setschedpolicy(pthread_attr_t *, int);
__attribute__((sysv_abi)) int pthread_attr_setscope(pthread_attr_t *, int);
__attribute__((sysv_abi)) int pthread_attr_setstack(pthread_attr_t *, void *, size_t);
__attribute__((sysv_abi)) int pthread_attr_setstacksize(pthread_attr_t *, size_t);

__attribute__((sysv_abi)) pthread_t pthread_self() {
    return GetCurrentThread();
}

__attribute__((sysv_abi)) int pthread_equal(pthread_t l, pthread_t r) {
    return l == r;
}

#include <process.h>

struct thread_args {
    __attribute__((sysv_abi)) void *(*start)(void*);
    void * user;
};
unsigned __stdcall pthread_run( void* pArguments ) {
    auto args = (thread_args*)pArguments;
    auto data = *args;
    delete args;
    return (unsigned)data.start(data.user);
}

__attribute__((sysv_abi)) int pthread_create(pthread_t* handle, const pthread_attr_t *attr, __attribute__((sysv_abi)) void *(*start)(void*), void * user) {
    *handle = (pthread_t)_beginthreadex(NULL, 0, &pthread_run, new thread_args{ start, user }, 0, nullptr);
    return 0;
}

__attribute__((sysv_abi)) int pthread_join(pthread_t thread, void ** exit) {
    WaitForSingleObject(thread, INFINITE);
    if(exit) {
        DWORD exitCode;
        GetExitCodeThread(thread, &exitCode);
        *exit = (void*)exitCode;
    }
    CloseHandle(thread);
    return 0;
}

auto ctr = ([]() -> int {
    setlocale(LC_ALL, ".UTF8");
    sizeof(std::shared_timed_mutex);
    // sht.lock_shared();
    return 0;
})();


template<class CH, size_t (cXrtomb)(char *_S, CH _C, mbstate_t *_Ps)>
std::string to_utf8_str(CH* str) {
    std::stringstream sout;
    mbstate_t state;
    char buf[MB_LEN_MAX];
    for(auto c = str; *c; c++) {
        size_t len = cXrtomb(buf, *c, &state);
        if(len != -1) {
            sout.write(buf, len);
        }
    }
    return sout.str();
}

int __cxa_atexit(void (*func) (void *), void * arg, void * dso_handle) {
    return 0;
}

const char* symbols[] = {
    "__cxa_finalize",
    "__cxa_atexit",
    "__sF",
    "__stack_chk_fail",
    "flockfile",
    "funlockfile",
    "putc_unlocked",
    "ferror",
    "memcpy",
    "memset",
    "getc_unlocked",
    "malloc",
    "realloc",
    "free",
    "gettid",
    "__libc_shim_open_3",
    "abort",
    "fputc",
    "android_set_abort_message",
    "openlog",
    "syslog",
    "closelog",
    "strlen",
    "memmove",
    "__memmove_chk",
    "__strlen_chk",
    "memchr",
    "posix_memalign",
    "pthread_mutex_lock",
    "pthread_mutex_unlock",
    "calloc",
    "strcmp",
    "pthread_getspecific",
    "pthread_once",
    "pthread_setspecific",
    "pthread_key_delete",
    "pthread_key_create",
    "fflush",
    "pthread_rwlock_wrlock",
    "pthread_rwlock_unlock",
    "dl_iterate_phdr",
    "pthread_rwlock_rdlock",
    "fwrite",
//NON LIBC SYMS
"acosf",
"cosf",
"sinf",
"setsockopt",
"fcntl",
"getaddrinfo",
"connect",
"getsockopt",
"freeaddrinfo",
"__errno",
"bind",
"listen",
"shutdown",
"log10f",
"powf",
"logf",
"expf",
"strncpy",
"asinf",
"strncmp",
"atan2f",
"__cxa_guard_acquire",
"__cxa_guard_release",
"ldexpf",
"exp2f",
"atoi",
"fseek",
"tanf",
"qsort",
"fopen",
"fclose",
"ftell",
"fread",
"feof",
"gettimeofday",
"usleep",
"pthread_self",
"setpriority",
"pthread_attr_init",
"pthread_attr_setdetachstate",
"pthread_attr_setstacksize",
"pthread_create",
"pthread_attr_destroy",
"pthread_mutex_destroy",
"sem_init",
"sem_destroy",
"sem_wait",
"sem_post",
"cos",
"sin",
"ldexp",
"pow",
"__assert2",
"log",
"fclose",
"sin",
"cos",
"acos",
"_ctype_",
"log10",
"longjmp",
"exp",
"strcasecmp",
"tanh",
"atan2",
"cbrt",
"fabs",
"floor",
"ceil",
"sqrt",
"asin",
"atan",
"tan",
"trunc",
"cosh",
"sinh",
"acosh",
"asinh",
"atanh",
"expm1",
"log1p",
"log2",
"cosf",
"strncmp",
"__memcpy_chk",
"strspn",
"atoi",
"strcspn",
"atof",
"sincosf",
"log",
"acosf",
"__android_log_print",
"strncpy",
"strftime",
"fopen",
"fread",
"fclose",
"feof",
"strrchr",
"AAssetManager_open",
"AAsset_getLength",
"AAsset_read",
"sin",
"__strncpy_chk",
"atanf",
"asinf",
"atan2f",
"tanf",
"mmap",
"madvise",
"mprotect",
"munmap",
"sinf",
"rand",
"pthread_create",
"__memset_chk",
"fmodf",
"cos",
"cbrtf",
"srand",
"exp2f",
"powf",
"hypotf",
"fmod",
"acos",
"expf",
"lround",
"signal",
"strptime",
"mktime",
"gmtime",
"bind",
"shutdown",
"recvfrom",
"inet_ntop",
"pow",
"logf",
"exp2",
"AAsset_getBuffer",
"AAsset_close",
"AInputEvent_getType",
"AInputEvent_getDeviceId",
"AMotionEvent_getAxisValue",
"AMotionEvent_getAction",
"AMotionEvent_getRawX",
"AMotionEvent_getRawY",
"AKeyEvent_getRepeatCount",
"AKeyEvent_getAction",
"AKeyEvent_getKeyCode",
"AKeyEvent_getMetaState",
"AMotionEvent_getX",
"AMotionEvent_getY",
"AMotionEvent_getHistorySize",
"AMotionEvent_getHistoricalX",
"AMotionEvent_getHistoricalY",
"AMotionEvent_getButtonState",
"sigaction",
"ALooper_pollAll",
"AInputQueue_getEvent",
"AInputQueue_preDispatchEvent",
"AInputQueue_finishEvent",
"ANativeWindow_getWidth",
"ANativeWindow_getHeight",
"ANativeActivity_finish",
"AInputEvent_getSource",
"AMotionEvent_getPointerId",
"AMotionEvent_getPointerCount",
"AAsset_seek64",
"AAsset_getLength64",
"AAsset_getRemainingLength64",
"getpagesize",
"AInputQueue_detachLooper",
"AInputQueue_attachLooper",
"AConfiguration_fromAssetManager",
"AConfiguration_getLanguage",
"AConfiguration_getCountry",
"pthread_cond_init",
"pipe",
"pthread_attr_init",
"pthread_attr_setdetachstate",
"AConfiguration_new",
"ALooper_prepare",
"ALooper_addFd",
"AConfiguration_delete",
"inet_pton",
"exit",
"setsockopt",
"sendto",
"nextafterf",
"log10",
"setjmp",
"longjmp",
"__strcpy_chk",
"getaddrinfo",
"freeaddrinfo",
"exp",
"nextafter",
"atoll",
"log10f",
"glBindRenderbuffer",
"glTexParameteri",
"glVertexAttribPointer",
"glDrawArrays",
"glRenderbufferStorage",
"glUniformMatrix4fv",
"glBindFramebuffer",
"glBufferData",
"glGetProgramInfoLog",
"glLinkProgram",
"glBlendFuncSeparate",
"glClearDepthf",
"glGetActiveUniform",
"glGetShaderiv",
"glClearStencil",
"glFramebufferTexture2D",
"glGenFramebuffers",
"glGetIntegerv",
"glGetShaderPrecisionFormat",
"glViewport",
"glDeleteProgram",
"glDisableVertexAttribArray",
"glAttachShader",
"glEnableVertexAttribArray",
"glGetAttribLocation",
"glGetError",
"glShaderSource",
"glUniform4fv",
"glUniformMatrix3fv",
"glBindTexture",
"glUniform1i",
"glCompressedTexImage2D",
"glEnable",
"glGetString",
"glCreateProgram",
"glBindBuffer",
"glColorMask",
"glGenerateMipmap",
"glFlush",
"glBufferSubData",
"glCompileShader",
"glScissor",
"glGenRenderbuffers",
"glTexImage2D",
"glDeleteFramebuffers",
"glTexParameterfv",
"glActiveTexture",
"glGetFloatv",
"glBlendColor",
"glDetachShader",
"glGetActiveAttrib",
"glGenBuffers",
"glGetProgramiv",
"glGetUniformLocation",
"glBlendEquationSeparate",
"glTexSubImage2D",
"glCreateShader",
"glDeleteTextures",
"glFrontFace",
"glGenTextures",
"glStencilOpSeparate",
"glDepthFunc",
"glDrawElements",
"glReadPixels",
"glDeleteShader",
"glCompressedTexSubImage2D",
"glDepthMask",
"glDeleteBuffers",
"glDisable",
"glPixelStorei",
"glUseProgram",
"glDeleteRenderbuffers",
"glUniform1iv",
"glCheckFramebufferStatus",
"glPolygonOffset",
"glStencilFuncSeparate",
"glClearColor",
"glTexParameterf",
"glClear",
"glCullFace",
"glFramebufferRenderbuffer",
"glGetShaderInfoLog",
"clearerr",
"isatty",
"strstr",
"strncasecmp",
"modf",
"roundf",
"modff",
"strpbrk",
"time",
"isspace",
"tolower",
"isalnum",
"frexpf",
"bsearch",
"strchr",
"wcsrtombs",
"getpid",
"_exit",
"getrusage",
"localtime_r",
"tmpfile",
"__android_log_vprint",
"prctl",
"pthread_getattr_np",
"pthread_attr_getstack",
"pthread_rwlock_init",
"pthread_rwlock_destroy",
"pthread_rwlock_tryrdlock",
"pthread_rwlock_trywrlock",
"sem_timedwait",
"getrlimit",
"fgets",
"scalbn",
"fseeko",
"ftello",
"pthread_kill",
"setvbuf",
"puts",
"unlink",
"fdopen",
"memalign",
"__android_log_write",
"fputs",
"rewind",
"tzset",
"strncat",
"__strncpy_chk2",
"mbstowcs",
"ioctl",
"inet_ntoa",
"gethostname",
"getsockname",
"__FD_SET_chk",
"__FD_ISSET_chk",
"inet_addr",
"getnameinfo",
"gethostbyname",
"pthread_attr_setschedparam",
"if_nametoindex",
"gai_strerror",
"if_indextoname",
"strnlen",
"round",
"clock",
"islower",
"isalpha",
"pthread_condattr_destroy",
"pthread_condattr_init",
"pthread_condattr_setclock",
"nan",
"pthread_setschedparam",
"sched_get_priority_max",
"sched_get_priority_min",
"toupper",
"__strchr_chk",
"__FD_CLR_chk",
"__cmsg_nxthdr",
"getpeername",
"recvmsg",
"pthread_attr_getschedparam",
"pthread_setname_np",
"sched_setaffinity",
"llround",
"sigemptyset",
"sigaddset",
"lseek",
"uname",
"sigprocmask",
"eglWaitClient",
"eglMakeCurrent",
"eglDestroySurface",
"eglDestroyContext",
"eglTerminate",
"eglGetDisplay",
"eglInitialize",
"eglQueryString",
"eglChooseConfig",
"eglGetConfigAttrib",
"ANativeWindow_setBuffersGeometry",
"eglCreateWindowSurface",
"eglGetError",
"eglSwapInterval",
"eglCreateContext",
"eglGetProcAddress",
"eglSwapBuffers",
"chmod",
"rmdir",
"utime",
"pthread_attr_getdetachstate",
"mallinfo",
"setenv",
"unsetenv",
"AAssetDir_close",
"AAssetDir_getNextFileName",
"AAssetManager_openDir",
"frexp",
"malloc_usable_size",
"fesetround",
"hypot",
"gmtime_r",
"wcstombs",
"access",
"lroundf",
"strlcpy",
"raise",
"__pthread_cleanup_pop",
"__pthread_cleanup_push",
"epoll_ctl",
"epoll_wait",
"iswspace",
"pause",
"pthread_atfork",
"getegid",
"geteuid",
"getgid",
"getuid",
"pthread_getschedparam",
        // "pthread_self",
// "clock_gettime",
// "__errno",
// "pthread_cond_signal",
// "pthread_cond_broadcast",
// "pthread_cond_wait",
// "pthread_cond_timedwait",
// "pthread_cond_destroy",
"readdir",
"closedir",
"opendir",
"realpath",
"lstat",
// "stat",
"symlink",
"link",
"mkdir",
"fchmod",
"ftruncate",
"sendfile",
"readlink",
"pathconf",
"getcwd",
"chdir",
"utimensat",
"fchmodat",
"remove",
"rename",
"truncate",
"statvfs",
"getenv",
"fstat",
"wmemcpy",
"ungetc",
"getc",
// "newlocale",
// "uselocale",
"isxdigit_l",
"isdigit_l",
"wmemset",
"strftime_l",
"mbsrtowcs",
"setlocale",
"freelocale",
"strcoll_l",
"strxfrm_l",
"wcscoll_l",
"wcsxfrm_l",
"iswlower_l",
"islower_l",
"isupper_l",
"toupper_l",
"tolower_l",
"iswspace_l",
"iswprint_l",
"iswblank_l",
"iswcntrl_l",
"iswupper_l",
"iswalpha_l",
"iswdigit_l",
"iswpunct_l",
"iswxdigit_l",
"towupper_l",
"towlower_l",
"btowc",
"wctob",
"wcsnrtombs",
"wcrtomb",
"mbsnrtowcs",
"mbrtowc",
// "mbtowc",
// "__ctype_get_mb_cur_max",
"mbrlen",
"wcslen",
"localeconv",
"strtoll_l",
"strtoull_l",
"strtof",
"strtod",
"strtold_l",
"sched_yield",
// "pthread_mutexattr_init",
// "pthread_mutexattr_settype",
// "pthread_mutex_init",
// "pthread_mutexattr_destroy",
// "pthread_mutex_destroy",
"pthread_equal",
"__read_chk",
"wmemmove",
"wmemchr",
"wmemcmp",
"strtoul",
"wcstoul",
"strtoll",
"wcstoll",
"strtoull",
"wcstoull",
"wcstof",
"wcstod",
"strtold",
"wcstold",
"strtol",
"wcstol",
// "strerror_r",
"pthread_join",
"pthread_detach",
"sysconf",
"nanosleep",
};

template<auto symbols, size_t i, size_t max> void InsertAbort(std::vector<mcpelauncher_hook_t>& hooks) {
    if constexpr(i < max) {
        auto obj = &aborthelper<symbols, i>::__myabort;
        hooks.emplace_back(mcpelauncher_hook_t{symbols[i], reinterpret_cast<void*>(obj)});
        InsertAbort<symbols, i + 1, max>(hooks);
    }
}

// template<size_t s> void InsertAbort<s, s>(std::vector<mcpelauncher_hook_t> &hooks) {

// }
// template<class T> struct Wrap;
// template<class R, class ...P> struct Wrap<R(P...)> {
//     __attribute__((sysv_abi)) R invoke(R(P...) f, P ...args) {
//         return f(args...);
//     }
// };

template<auto F, class T> struct Impl;
template<auto F, class R, class ...P> struct Impl<F, R(P...)> {
    __attribute__((sysv_abi)) static R invoke(P ...args) {
        return F(args...);
    }
};
template<auto F, class R, class ...P> struct Impl<F, R(*)(P...)> : Impl<F, R(P...)> {};

template<auto F> struct Wrap : Impl<F, decltype(F)> {
    using Impl<F, decltype(F)>::invoke;
};

#include <GLFW/glfw3.h>
#include <math.h>

int __fgetwc(FILE* file) {
    _lock_file(file);
    defer d { [&]() {
        _unlock_file(file);
    } };
    wint_t lc = _fgetwc_nolock(file);
    if(lc == (wint_t)EOF) {
        return EOF;
    }
    mbstate_t state;
    char buf[MB_LEN_MAX];
    int lcc = c16rtomb(buf, lc, &state);
    if(lcc > 0) {
        return (int)lc;
    }
    wint_t rc = _fgetwc_nolock(file);
    if(rc == (wint_t)EOF) {
        return EOF;
    }
    int rcc = c16rtomb(buf, lc, &state);
    char32_t mbs = 0;
    mbrtoc32(&mbs, buf, rcc, &state);
    return mbs;
}

int __fputwc(char32_t ch, FILE* file) {
    _lock_file(file);
    defer d { [&]() {
        _unlock_file(file);
    } };
    char buf[MB_LEN_MAX];
    mbstate_t state;
    int size = c32rtomb(buf, ch, &state);
    char16_t c16;
    int llc = mbrtoc16(&c16, buf, size, &state);
    _fputwc_nolock(c16, file);
    if(llc == 0) {
        mbrtoc16(&c16, buf, size, &state);
        _fputwc_nolock(c16, file);
    }
    return 1;
}


int __ungetwc(char32_t ch, FILE* file) {
    _lock_file(file);
    defer d { [&]() {
        _unlock_file(file);
    } };
    char buf[MB_LEN_MAX];
    mbstate_t state;
    int size = c32rtomb(buf, ch, &state);
    char16_t c16;
    int llc = mbrtoc16(&c16, buf, size, &state);
    _ungetwc_nolock(c16, file);
    if(llc == 0) {
        mbrtoc16(&c16, buf, size, &state);
        _ungetwc_nolock(c16, file);
    }
    return 1;
}

// #include <handleapi.h>
// #define _WIN32_WINNT 0x6000


int __close(int fd) {
    // HANDLE dup;
    // BOOL dupped = DuplicateHandle(GetCurrentProcess(), 
    //                 (HANDLE)_get_osfhandle(fd), 
    //                 GetCurrentProcess(),
    //                 &dup,
    //                 0,
    //                 FALSE,
    //                 DUPLICATE_SAME_ACCESS);
    // int ret = _close(fd);
    // if(dupped && closesocket((SOCKET)dup) && WSAGetLastError() == WSAENOTSOCK) {
    //     CloseHandle(dup);
    // }
    closesocket((SOCKET)_get_osfhandle(fd));
    WSASetLastError(0);
    return _close(fd);
}

namespace sconvwrap {
template<class T> struct conv {
    static T from(T s) {
        return s;
    }
    static T to(T s) {
        return s;
    }
};
template<> struct conv<SOCKET> {
    static int from(SOCKET s) {
        return _open_osfhandle((intptr_t)s, 0);
    }
    static SOCKET to(int s) {
        return (SOCKET)_get_osfhandle(s);
    }
};

template<auto F, class T> struct Impl;
template<auto F, class R, class ...P> struct Impl<F, R(P...)> {
    __attribute__((sysv_abi)) static R invoke(P ...args) {
        return conv<R>::from(F(conv<P>::to(args)...));
    }
};
template<auto F, class ...P> struct Impl<F, void(P...)> {
    __attribute__((sysv_abi)) static void invoke(P ...args) {
        F(conv<P>::to(args)...);
    }
};
template<auto F, class R, class ...P> struct Impl<F, R(*)(P...)> : Impl<F, R(P...)> {};

template<auto F> struct Wrap : Impl<F, decltype(F)> {
    using Impl<F, decltype(F)>::invoke;
};
}

namespace convwrap {
template<class T> struct conv {
    static T from(T s) {
        return s;
    }
    static T to(T s) {
        return s;
    }
};

char __sF[152 * 3];
template<> struct conv<FILE*> {
    static FILE* from(FILE* s) {
        return s;
    }
    static FILE* to(FILE* s) {
        if((char*)s == __sF) {
            return stdin;
        } else if((char*)s == (__sF + 152)) {
            return stdout;
        } else if((char*)s == (__sF + 152 * 2)) {
            return stderr;
        }
        return s;
    }
};

// ((char*)stdout) - 152

template<auto F, class T> struct Impl;
template<auto F, class R, class ...P> struct Impl<F, R(P...)> {
    __attribute__((sysv_abi)) static R invoke(P ...args) {
        return conv<R>::from(F(conv<P>::to(args)...));
    }
};
template<auto F, class ...P> struct Impl<F, void(P...)> {
    __attribute__((sysv_abi)) static void invoke(P ...args) {
        F(conv<P>::to(args)...);
    }
};
template<auto F, class R, class ...P> struct Impl<F, R(*)(P...)> : Impl<F, R(P...)> {};

template<auto F> struct Wrap : Impl<F, decltype(F)> {
    using Impl<F, decltype(F)>::invoke;
};
}

long long gettid() {
    return GetCurrentThreadId();
}
#include <locale.h>

_locale_t newlocale(int category_mask, const char *locale, _locale_t base) {
    return _create_locale(category_mask, locale);
}

_locale_t uselocale(_locale_t l) {
    return l;
}

size_t __ctype_get_mb_cur_max() {
    return MB_CUR_MAX;
}

int __mbtowc(char32_t* wchar, const char *mbchar, size_t count) {
    mbstate_t state;
    int llc = mbrtoc32(wchar, mbchar, count, &state);
    if(llc == 0) {
        int rrc = mbrtoc32(wchar, mbchar, count, &state);
        return rrc;
    }
    return llc;
}

int getpagesize(void) {
    return 4096;
}

//LIBCSHIM

char* __strcpy_chk(char* dst, const char* src, size_t dst_len) {
  return strcpy(dst, src);
}

char* __strcat_chk(char *dst, const char *src, size_t dst_len) {
    return strcat(dst, src);
}

char* __strncat_chk(char *dst, const char *src, size_t n, size_t dst_len) {
    return strncat(dst, src, n);
}

char* __strncpy_chk(char* dst, const char* src, size_t len, size_t dst_len) {
  return strncpy(dst, src, len);
}

char* __strncpy_chk2(char* dst, const char* src, size_t n, size_t dst_len, size_t src_len) {
    return strncpy(dst, src, n);
}

size_t strlen_chk(const char *str, size_t max_len) {
    auto ret = strlen(str);
    if (ret >= max_len) {
        fprintf(stderr, "strlen_chk: string longer than expected\n");
        abort();
    }
    return ret;
}

const char* strchr_chk(const char* __s, int __ch, size_t __n) {
    return strchr(__s, __ch);
}

void* __memcpy_chk(void *dst, const void *src, size_t size, size_t max_len) {
    if (size > max_len) {
        fprintf(stderr, "detected copy past buffer size");
        abort();
    }
    return ::memcpy(dst, src, size);
}

void* __memmove_chk(void *dst, const void *src, size_t size, size_t max_len) {
    if (size > max_len) {
        fprintf(stderr, "detected copy past buffer size");
        abort();
    }
    return ::memmove(dst, src, size);
}

void* __memset_chk(void *dst, int c, size_t size, size_t max_len) {
    if (size > max_len) {
        fprintf(stderr, "detected set past buffer size");
        abort();
    }
    return ::memset(dst, c, size);
}

size_t __read_chk(int fd, void *buf, size_t count, size_t buf_size) {
    return read(fd, buf, count);
}

//END LIBCSHIM

#include <thread>
#include <random>

std::string wrapPath(const char *path) {
    char tmp[260];
    GetTempPathA(sizeof(tmp) - 1, tmp);
    return std::string(tmp) + "\\" + path;
}

int __open_3(const char* path, int flag, int mode) {
    if(!strcmp(path, "/dev/random") || !strcmp(path, "/dev/urandom")) {
        auto handles = new int[2];
        _pipe(handles, 2, _O_BINARY);
        std::thread([handles]() {
            std::random_device rd;
            std::uniform_int_distribution<int> randomize(0, 255);
            while(true) {
                uint8_t val = (uint8_t)randomize(rd);
                if(_write(handles[1], &val, 1) != 1) {
                    break;
                }
            }
            _close(handles[1]);
            delete[] handles;
        }).detach();
        return handles[0];
    }
    return _open(wrapPath(path).data(), flag, mode);
}

int __open_2(const char* path, int flag) {
    return __open_3(path, flag, 0);
}

FILE* __fopen(const char* path, const char *mode) {
    return fopen(wrapPath(path).data(), mode);
}

int __setvbuf(FILE *stream, char * buffer, int mode, size_t size) {
    int hmode = 0;
    switch(mode) {
        case 0:
            hmode = _IOFBF;
        case 1:
            hmode = _IOLBF;
        case 2:
            hmode = _IONBF;
    }
    return setvbuf(stream, buffer, hmode, size);
}

struct bionic_stat {
    unsigned long long st_dev;
    unsigned long long st_ino;
    unsigned long long st_nlink;
    unsigned int st_mode;
    unsigned int st_uid;
    unsigned int st_gid;
    unsigned int __pad0;
    unsigned long long st_rdev;
    long long st_size;
    long long st_blksize;
    long long st_blocks;
    bionic_timespec st_atim;
    bionic_timespec st_mtim;
    bionic_timespec st_ctim;
    long long __pad3[3];
};

int __stat(const char * path, struct bionic_stat * statbuf) {
    struct _stat64 st;
    int r = _stat64(wrapPath(path).data(), &st);
    if(r == 0) {
        memset(statbuf, 0, sizeof(bionic_stat));
        statbuf->st_dev = st.st_dev;
        statbuf->st_ino = st.st_ino;
        statbuf->st_nlink = st.st_nlink;
        statbuf->st_mode = st.st_mode;
        statbuf->st_uid = st.st_uid;
        statbuf->st_gid = st.st_gid;
        statbuf->st_rdev = st.st_rdev;
        statbuf->st_size = st.st_size;
        statbuf->st_blksize = 1;
        statbuf->st_blocks = st.st_size;
        statbuf->st_atim = { st.st_atime, 0 };
        statbuf->st_ctim = { st.st_ctime, 0 };
        statbuf->st_mtim = { st.st_mtime, 0 };
    }
    return r;
}
int __fstat(int fd, struct bionic_stat *statbuf) {
    struct _stat64 st;
    int r = _fstat64(fd, &st);
    if(r == 0) {
        memset(statbuf, 0, sizeof(bionic_stat));
        statbuf->st_dev = st.st_dev;
        statbuf->st_ino = st.st_ino;
        statbuf->st_nlink = st.st_nlink;
        statbuf->st_mode = st.st_mode;
        statbuf->st_uid = st.st_uid;
        statbuf->st_gid = st.st_gid;
        statbuf->st_rdev = st.st_rdev;
        statbuf->st_size = st.st_size;
        statbuf->st_blksize = 1;
        statbuf->st_blocks = st.st_size;
        statbuf->st_atim = { st.st_atime, 0 };
        statbuf->st_ctim = { st.st_ctime, 0 };
        statbuf->st_mtim = { st.st_mtime, 0 };
    }
    return r;
}
#include <filesystem>
int __lstat(const char * path, struct bionic_stat * statbuf) {
    // std::filesystem::is_symlink(wrapPath(path));
    return __stat(path, statbuf);
}

#include <direct.h>

int __mkdir(const char *path, int mode) {
    return mkdir(wrapPath(path).data());
}

int posix_memalign(void **memptr, size_t alignment, size_t size) {
    if(memptr) {
        *memptr = _aligned_malloc(size+128, alignment);
        if(*memptr != nullptr) {
            return 0;
        }
    }
    return 1;
}

void *__malloc(size_t _Size) {
    return _aligned_malloc(_Size+128, 1);
}

void *__realloc(void *_Block, size_t _Size) {
    return _aligned_realloc(_Block, _Size+128, 1);
}

void __free(void *_Block) {
    _aligned_free(_Block);
}

void *__calloc(size_t number, size_t _Size) {
    return _aligned_malloc(number * _Size+128, 1);
}

int strerror_r(int __errno_value, char* __buf, size_t __n) {
    return strerror_s(__buf, __n, __errno_value);
}

struct tm *localtime_r( const time_t *timer, struct tm *buf ) {
    if(localtime_s(buf, timer)) {
        return nullptr;
    }
    return buf;
}

int sigaction(int signum, const struct sigaction * act, struct sigaction * oldact) {
    return 0;
}

#include "native_activity.h"

// struct fd_pair {
//     long fd[2];
// };
// struct fd_pair pipe() {
//     fd_pair pair;
//     _pipe(pair.fd, 512, _O_BINARY);
//     return pair;
// }
int pipe(int* pipefd) {
    return _pipe(pipefd, 512, _O_BINARY);
}

void android_set_abort_message(const char *msg) {
    fprintf(stderr, "abort message: %s\n", msg);
}

struct AAsset {
    int fd;
};
AAsset * AAssetManager_open(void *mgr, const char *filename, int mode) {
    auto aasset = new AAsset();
    aasset->fd = _open((std::string(".\\1.20.0.01\\assets\\") + filename).data(), O_RDONLY);
    return aasset;
}

int64_t AAsset_getLength(AAsset *asset) {
    long ret = lseek(asset->fd, 0, SEEK_END);
    lseek(asset->fd, 0, SEEK_SET);
    return ret;
}

int AAsset_read(AAsset *asset, void *buf, size_t count) {
    return read(asset->fd, buf, count);
}

int wmain(int argc, const wchar_t **wargv) {
    // glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_TRUE);
    // glfwInit();
    // static int glfwjid = -1;
    // glfwSetJoystickCallback([](int jid, int event) {
    //     if(event == GLFW_CONNECTED && glfwJoystickIsGamepad(jid)) {
    //         glfwjid = jid;
    //     } else if(event == GLFW_DISCONNECTED && jid == glfwjid) {
    //         glfwjid = -1;
    //     } 
    // });
    // while(true) {
    //     if(glfwjid != -1) {
    //         // int btns;
    //         // const unsigned char * b = glfwGetJoystickButtons(glfwjid, &btns);
    //         // int axes;
    //         // const float * a = glfwGetJoystickAxes(glfwjid, &axes);
    //         if(GLFWgamepadstate state; glfwGetGamepadState(glfwjid, &state) == GLFW_TRUE) {
    //             if(state.buttons[GLFW_GAMEPAD_BUTTON_A]) {
    //                 printf("GLFW_GAMEPAD_BUTTON_A pressed\n");
    //             }
    //             if(state.buttons[GLFW_GAMEPAD_BUTTON_B]) {
    //                 printf("GLFW_GAMEPAD_BUTTON_B pressed exit\n");
    //                 return 0;
    //             }
    //         }
    //         glfwWaitEventsTimeout(0.02);
    //     } else {
    //         glfwWaitEvents();
    //     }
    // }
    std::vector<std::string> argv(argc);

    _set_fmode(_O_BINARY);
    // // auto name = to_utf8_str<wchar_t, &wcrtomb>(_wgetenv(L"TEST"));
    // // auto name = to_utf8_str<char16_t, &c16rtomb>((char16_t *)_wgetenv(L"TEST"));
    // // creat(name.data(), _S_IREAD | _S_IWRITE);

    // FILE *fp = fopen("Hello.txt", "w"); //UTF-16LE //UNICODE , ccs=UTF-8
    // // fputws(L"Mee 😊 Hello.txt\n next line 😊", fp);
    // fputs("Mee 😊 Hello.txt\n next line 😊", fp);
    // fclose(fp);
    // return 0;
    WSADATA d;
    WSAStartup(WINSOCK_VERSION, &d);
    linker::init();
    std::unordered_map<std::string, void *> syms;
    syms["__cxa_finalize"] = reinterpret_cast<void*>(&__abort);
    syms["__cxa_atexit"] = reinterpret_cast<void*>(&__abort);
    syms["print"] = reinterpret_cast<void*>(&print);
    syms["__stack_chk_fail"] = reinterpret_cast<void*>(&__abort);
    // for(auto&& sym : symbols) {
    //     std::string name = sym;
    //     if(syms.find(name) == syms.end()) {
    //         syms[name] = reinterpret_cast<void*>(&__abort);
    //     }
    // }
    linker::load_library("libm.so", {});
    // linker::load_library("liblog.so", {});

    linker::load_library("libEGL.so", {});
    linker::load_library("libGLESv2.so", {});
    linker::load_library("libstdc++.so", {});
    linker::load_library("libandroid.so", {});
    
    
    android_dlextinfo extinfo;
    std::vector<mcpelauncher_hook_t> hooks;
    // for(auto&& sym : symbols) {
    //     hooks.emplace_back(mcpelauncher_hook_t{sym, reinterpret_cast<void*>(&__abort)});
    // }
    // auto obj = &aborthelper<2>::__myabort;
    InsertAbort<symbols, 0, sizeof(symbols) / sizeof(*symbols)>(hooks);

    hooks.emplace_back(mcpelauncher_hook_t{"print", reinterpret_cast<void*>(&print)});
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_mutexattr_init", reinterpret_cast<void*>(&pthread_mutexattr_init)});
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_mutexattr_destroy", reinterpret_cast<void*>(&pthread_mutexattr_destroy)});
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_mutexattr_settype", reinterpret_cast<void*>(&pthread_mutexattr_settype)});
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_mutex_init", reinterpret_cast<void*>(&pthread_mutex_init)});
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_mutex_lock", reinterpret_cast<void*>(&pthread_mutex_lock)});
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_mutex_trylock", reinterpret_cast<void*>(&pthread_mutex_trylock)});
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_mutex_unlock", reinterpret_cast<void*>(&pthread_mutex_unlock)});
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_mutex_destroy", reinterpret_cast<void*>(&pthread_mutex_destroy)});
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_condattr_init", reinterpret_cast<void*>(&pthread_cond_init)});
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_condattr_destroy", reinterpret_cast<void*>(&pthread_cond_destroy)});
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_condattr_setclock", reinterpret_cast<void*>(&pthread_condattr_setclock)});
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_condattr_getclock", reinterpret_cast<void*>(&pthread_condattr_getclock)});
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_condattr_setpshared", reinterpret_cast<void*>(&pthread_condattr_setpshared)});
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_condattr_getpshared", reinterpret_cast<void*>(&pthread_condattr_getpshared)});
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_cond_init", reinterpret_cast<void*>(&pthread_cond_init)});
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_cond_destroy", reinterpret_cast<void*>(&pthread_cond_destroy)});
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_cond_broadcast", reinterpret_cast<void*>(&pthread_cond_broadcast)});
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_cond_signal", reinterpret_cast<void*>(&pthread_cond_signal)});
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_cond_timedwait", reinterpret_cast<void*>(&pthread_cond_timedwait)});
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_cond_wait", reinterpret_cast<void*>(&pthread_cond_wait)});
    
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_key_create", reinterpret_cast<void*>(&pthread_key_create)});
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_getspecific", reinterpret_cast<void*>(&pthread_getspecific)});
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_setspecific", reinterpret_cast<void*>(&pthread_setspecific)});
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_once", reinterpret_cast<void*>(&pthread_once)});
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_self", reinterpret_cast<void*>(&pthread_self)});
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_equal", reinterpret_cast<void*>(&pthread_equal)});
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_create", reinterpret_cast<void*>(&pthread_create)});
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_join", reinterpret_cast<void*>(&pthread_join)});
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_attr_init", reinterpret_cast<void*>(&pthread_attr_init)});
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_attr_destroy", reinterpret_cast<void*>(&pthread_attr_destroy)});
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_attr_setdetachstate", reinterpret_cast<void*>(&pthread_attr_setdetachstate)});
    // hooks.emplace_back(mcpelauncher_hook_t{"pthread_attr_getstack", reinterpret_cast<void*>(&pthread_attr_getstack)});
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_rwlock_init", reinterpret_cast<void*>(&pthread_rwlock_init)});
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_rwlock_destroy", reinterpret_cast<void*>(&pthread_rwlock_destroy)});
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_rwlock_tryrdlock", reinterpret_cast<void*>(&pthread_rwlock_tryrdlock)});
    hooks.emplace_back(mcpelauncher_hook_t{"pthread_rwlock_trywrlock", reinterpret_cast<void*>(&pthread_rwlock_trywrlock)});

    // memchr()
    hooks.emplace_back(mcpelauncher_hook_t{"memcpy", reinterpret_cast<void*>(&Wrap<memcpy>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"memchr", reinterpret_cast<void*>(&Wrap<(const void * (*)(const void *_Buf, int _Val, size_t _MaxCount))&memchr>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"memmove", reinterpret_cast<void*>(&Wrap<memmove>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"memcmp", reinterpret_cast<void*>(&Wrap<memcmp>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"memset", reinterpret_cast<void*>(&Wrap<memset>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"strcpy", reinterpret_cast<void*>(&Wrap<strcpy>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"strncpy", reinterpret_cast<void*>(&Wrap<strncpy>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"strcmp", reinterpret_cast<void*>(&Wrap<strcmp>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"strncmp", reinterpret_cast<void*>(&Wrap<strncmp>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"strchr", reinterpret_cast<void*>(&Wrap<(char *(*)( char *str, int ch ))strchr>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"strrchr", reinterpret_cast<void*>(&Wrap<(char *(*)( char *str, int ch ))strrchr>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"strdup", reinterpret_cast<void*>(&Wrap<strdup>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"strcat", reinterpret_cast<void*>(&Wrap<strcat>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"strlen", reinterpret_cast<void*>(&Wrap<strlen>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"strerror", reinterpret_cast<void*>(&Wrap<strerror>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"perror", reinterpret_cast<void*>(&Wrap<perror>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"fopen", reinterpret_cast<void*>(&convwrap::Wrap<__fopen>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"fdopen", reinterpret_cast<void*>(&convwrap::Wrap<_fdopen>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"fread", reinterpret_cast<void*>(&convwrap::Wrap<fread>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"fwrite", reinterpret_cast<void*>(&convwrap::Wrap<fwrite>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"feof", reinterpret_cast<void*>(&convwrap::Wrap<feof>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"fclose", reinterpret_cast<void*>(&convwrap::Wrap<fclose>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"flockfile", reinterpret_cast<void*>(&convwrap::Wrap<_lock_file>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"funlockfile", reinterpret_cast<void*>(&convwrap::Wrap<_unlock_file>::invoke)});
    // TODO look into libc-shim
    hooks.emplace_back(mcpelauncher_hook_t{"__sF", reinterpret_cast<void*>(convwrap::__sF)});
    hooks.emplace_back(mcpelauncher_hook_t{"getc_unlocked", reinterpret_cast<void*>(&convwrap::Wrap<_getc_nolock>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"putc_unlocked", reinterpret_cast<void*>(&convwrap::Wrap<_putc_nolock>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"fputc", reinterpret_cast<void*>(&convwrap::Wrap<fputc>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"putc", reinterpret_cast<void*>(&convwrap::Wrap<putc>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"fflush", reinterpret_cast<void*>(&convwrap::Wrap<fflush>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"fileno", reinterpret_cast<void*>(&convwrap::Wrap<fileno>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"ferror", reinterpret_cast<void*>(&convwrap::Wrap<ferror>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"fgetwc", reinterpret_cast<void*>(&convwrap::Wrap<__fgetwc>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"fputwc", reinterpret_cast<void*>(&convwrap::Wrap<__fputwc>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"ungetwc", reinterpret_cast<void*>(&convwrap::Wrap<ungetwc>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"setvbuf", reinterpret_cast<void*>(&convwrap::Wrap<__setvbuf>::invoke)});
//     _IOFBF
// _IOLBF	line buffering
// _IONBF	no buffering
    hooks.emplace_back(mcpelauncher_hook_t{"close", reinterpret_cast<void*>(&Wrap<__close>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"__libc_shim_open_3", reinterpret_cast<void*>(&Wrap<__open_3>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"__open_2", reinterpret_cast<void*>(&Wrap<__open_2>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"stat", reinterpret_cast<void*>(&Wrap<__stat>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"lstat", reinterpret_cast<void*>(&Wrap<__lstat>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"fstat", reinterpret_cast<void*>(&Wrap<__fstat>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"mkdir", reinterpret_cast<void*>(&Wrap<__mkdir>::invoke)});
    
    hooks.emplace_back(mcpelauncher_hook_t{"read", reinterpret_cast<void*>(&Wrap<_read>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"__read_chk", reinterpret_cast<void*>(&Wrap<__read_chk>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"write", reinterpret_cast<void*>(&Wrap<_write>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"socket", reinterpret_cast<void*>(&sconvwrap::Wrap<socket>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"recv", reinterpret_cast<void*>(&sconvwrap::Wrap<recv>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"send", reinterpret_cast<void*>(&sconvwrap::Wrap<send>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"accept", reinterpret_cast<void*>(&sconvwrap::Wrap<accept>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"poll", reinterpret_cast<void*>(&sconvwrap::Wrap<WSAPoll>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"select", reinterpret_cast<void*>(&sconvwrap::Wrap<select>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"gettid", reinterpret_cast<void*>(&Wrap<gettid>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"newlocale", reinterpret_cast<void*>(&Wrap<newlocale>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"uselocale", reinterpret_cast<void*>(&Wrap<uselocale>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"freelocale", reinterpret_cast<void*>(&Wrap<_free_locale>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"__ctype_get_mb_cur_max", reinterpret_cast<void*>(&Wrap<__ctype_get_mb_cur_max>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"mbtowc", reinterpret_cast<void*>(&Wrap<__mbtowc>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"__cxa_atexit", reinterpret_cast<void*>(&Wrap<__cxa_atexit>::invoke)});
    // MALLOC
    // hooks.emplace_back(mcpelauncher_hook_t{"malloc", reinterpret_cast<void*>(&Wrap<malloc>::invoke)});
    // hooks.emplace_back(mcpelauncher_hook_t{"realloc", reinterpret_cast<void*>(&Wrap<realloc>::invoke)});
    // hooks.emplace_back(mcpelauncher_hook_t{"free", reinterpret_cast<void*>(&Wrap<free>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"posix_memalign", reinterpret_cast<void*>(&Wrap<posix_memalign>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"malloc", reinterpret_cast<void*>(&Wrap<__malloc>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"realloc", reinterpret_cast<void*>(&Wrap<__realloc>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"free", reinterpret_cast<void*>(&Wrap<__free>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"calloc", reinterpret_cast<void*>(&Wrap<__calloc>::invoke)});

    
    //OTHER
    hooks.emplace_back(mcpelauncher_hook_t{"getpagesize", reinterpret_cast<void*>(&Wrap<getpagesize>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"nextafter", reinterpret_cast<void*>(&Wrap<_nextafter>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"nextafterf", reinterpret_cast<void*>(&Wrap<_nextafterf>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"exp2f", reinterpret_cast<void*>(&Wrap<exp2f>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"exp2", reinterpret_cast<void*>(&Wrap<(double(*)(double))exp2>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"sinf", reinterpret_cast<void*>(&Wrap<sinf>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"sin", reinterpret_cast<void*>(&Wrap<(double(*)(double))sin>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"clock_gettime", reinterpret_cast<void*>(clock_gettime)});
    hooks.emplace_back(mcpelauncher_hook_t{"gettimeofday", reinterpret_cast<void*>(gettimeofday)});
    hooks.emplace_back(mcpelauncher_hook_t{"__errno", reinterpret_cast<void*>(&Wrap<_errno>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"strerror_r", reinterpret_cast<void*>(&Wrap<strerror_r>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"time", reinterpret_cast<void*>(&Wrap<time>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"localtime_r", reinterpret_cast<void*>(&Wrap<localtime_r>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"strftime", reinterpret_cast<void*>(&Wrap<strftime>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"sigaction", reinterpret_cast<void*>(&Wrap<sigaction>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"pipe", reinterpret_cast<void*>(&Wrap<pipe>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"android_set_abort_message", reinterpret_cast<void*>(&Wrap<android_set_abort_message>::invoke)});

    // FORTIFY

    hooks.emplace_back(mcpelauncher_hook_t{"__strcpy_chk", reinterpret_cast<void*>(&Wrap<__strcpy_chk>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"__strcat_chk", reinterpret_cast<void*>(&Wrap<__strcat_chk>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"__strncat_chk", reinterpret_cast<void*>(&Wrap<__strncat_chk>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"__strncpy_chk", reinterpret_cast<void*>(&Wrap<__strncpy_chk>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"__strncpy_chk2", reinterpret_cast<void*>(&Wrap<__strncpy_chk2>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"__strlen_chk", reinterpret_cast<void*>(&Wrap<strlen_chk>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"__strchr_chk", reinterpret_cast<void*>(&Wrap<strchr_chk>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"__memcpy_chk", reinterpret_cast<void*>(&Wrap<__memcpy_chk>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"__memmove_chk", reinterpret_cast<void*>(&Wrap<__memmove_chk>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"__memset_chk", reinterpret_cast<void*>(&Wrap<__memset_chk>::invoke)});
  
    // END
    
    //libandroid


    hooks.emplace_back(mcpelauncher_hook_t{"AAssetManager_open", reinterpret_cast<void*>(&Wrap<AAssetManager_open>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"AAsset_getLength", reinterpret_cast<void*>(&Wrap<AAsset_getLength>::invoke)});
    hooks.emplace_back(mcpelauncher_hook_t{"AAsset_read", reinterpret_cast<void*>(&Wrap<AAsset_read>::invoke)});
    
    struct Looper {
      int fdin;
      int fdout;
      int indent;
      void * data;
      int indent2;
      void * data2;
      char buf[10];
      OVERLAPPED alooperov;
    };
    static Looper looper;
    static constexpr auto l0 = +[](int timeoutMillis, int *outFd, int *outEvents, void **outData) {
      fd_set rfds;
      struct timeval tv;
      DWORD retval;
      ResetEvent(looper.alooperov.hEvent);
      DWORD length;
      BOOL suc = ReadFile((HANDLE)_get_osfhandle(looper.fdin), &looper.buf, 10, &length, &looper.alooperov);
      DWORD err = GetLastError();
      if(err) {
          std::cout << "err: " << err << "\n";
          BOOL ret = GetOverlappedResult((HANDLE)_get_osfhandle(looper.fdin), &looper.alooperov, &length, FALSE);
          err = GetLastError();
          std::cout << "err: " << err << "\n";
      }
      if (!err) {
          *outData = looper.data;
          _write(looper.fdout, looper.buf, length);
          return looper.indent;
      }

      return -3;
    };
    hooks.emplace_back(mcpelauncher_hook_t{"ALooper_pollAll", (void *)Wrap<l0>::invoke});
    static constexpr auto l1 = +[](void *loopere, int fd, int ident, int events, int(*callback)(int fd, int events, void *data), void *data) {
      int fdin = _dup(fd);
      int fd_[2];
      _pipe(fd_, 128, O_BINARY | _O_NOINHERIT);
      _dup2(fd_[0], fd);
      looper.fdin = fdin;
      looper.fdout = fd_[1];
      looper.indent = ident;
      looper.data = data;
      looper.alooperov.hEvent = CreateEventW(NULL, TRUE, TRUE, NULL);
      return 1;
    };
    hooks.emplace_back(mcpelauncher_hook_t{"ALooper_addFd", (void *)Wrap<l1>::invoke});
    static constexpr auto l2 = +[](void *queue, void *looper2, int ident, void* callback, void *data) {
      looper.indent2 = ident;
      looper.data2 = data;
    };
    hooks.emplace_back(mcpelauncher_hook_t{"AInputQueue_attachLooper", (void *)Wrap<l2>::invoke});

    //END

    hooks.emplace_back(mcpelauncher_hook_t{nullptr, nullptr});
    extinfo.flags = ANDROID_DLEXT_MCPELAUNCHER_HOOKS;
    extinfo.mcpelauncher_hooks = hooks.data();
    if(linker::dlopen_ext(".\\libc.so", 0, &extinfo) == nullptr) {
        printf("LAUNCHER  Failed to load compat libc.so Original Error: %s", linker::dlerror());
        return 1;
    }
    linker::dlopen(".\\liblog.so", {});

    hooks.clear();

    // static const char* libcxxsyms[] = {

    // };

    // InsertAbort<libcxxsyms, 0, sizeof(libcxxsyms) / sizeof(*libcxxsyms)>(hooks);

    hooks.emplace_back(mcpelauncher_hook_t{nullptr, nullptr});
    extinfo.flags = ANDROID_DLEXT_MCPELAUNCHER_HOOKS;
    extinfo.mcpelauncher_hooks = hooks.data();
    if(linker::dlopen_ext(".\\1.20.0.01\\lib\\x86_64\\libc++_shared.so", 0, &extinfo) == nullptr) {
        printf("LAUNCHER  Failed to load compat libc.so Original Error: %s", linker::dlerror());
        return 1;
    }

    hooks.clear();

    static const char* libfmod[] = {
        "dummy"
    };

    InsertAbort<libfmod, 0, sizeof(libfmod) / sizeof(*libfmod)>(hooks);

    hooks.emplace_back(mcpelauncher_hook_t{nullptr, nullptr});
    extinfo.flags = ANDROID_DLEXT_MCPELAUNCHER_HOOKS;
    extinfo.mcpelauncher_hooks = hooks.data();
    if(linker::dlopen_ext(".\\1.20.0.01\\lib\\x86_64\\libfmod.so", 0, &extinfo) == nullptr) {
        printf("LAUNCHER  Failed to load compat libc.so Original Error: %s", linker::dlerror());
        return 1;
    }

    hooks.clear();

    static const char* libminecraftpe[] = {
        // "_ZN4bgfx4initERKNS_4InitE",
        // "_ZN4bgfx9initAsyncERKNS_4InitE"
    };

    InsertAbort<libminecraftpe, 0, sizeof(libminecraftpe) / sizeof(*libminecraftpe)>(hooks);

    hooks.emplace_back(mcpelauncher_hook_t{nullptr, nullptr});
    extinfo.flags = ANDROID_DLEXT_MCPELAUNCHER_HOOKS;
    extinfo.mcpelauncher_hooks = hooks.data();
    auto minecraft = linker::dlopen_ext(".\\1.20.0.01\\lib\\x86_64\\libminecraftpe.so", 0, &extinfo);
    if(minecraft == nullptr) {
        printf("LAUNCHER  Failed to load minecraft Original Error: %s", linker::dlerror());
        return 1;
    }
    int(__attribute__((sysv_abi)) *JNI_OnLoad)(void *vm, void *reserved) = (decltype(JNI_OnLoad))linker::dlsym(minecraft, "JNI_OnLoad");
    
    // linker::load_library("libc.so", syms);
    auto handle = linker::dlopen(".\\b-ndk\\libmcpelauncherupdates.so", 0);
    if(!handle) {
        printf("linker::dlerror %s\n", linker::dlerror());
        return 1;
    }
    auto GetJavaVM = (JavaVM*(__attribute__((sysv_abi))*)())linker::dlsym(handle, "GetJavaVM");

    auto now = std::chrono::steady_clock::now();
    bionic_timespec abs_timeout;
    clock_gettime(0, &abs_timeout);
    printf("time_since_epoch %lld\n", now.time_since_epoch().count());
    // std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds> tp;
    // tp += std::chrono::nanoseconds(abs_timeout.tv_sec);
    // tp += std::chrono::seconds(abs_timeout.tv_nsec);
    auto vm = GetJavaVM();


    ANativeActivity activity;
    memset(&activity, 0, sizeof(ANativeActivity));
    activity.internalDataPath = "./idata/";
    activity.externalDataPath = "./edata/";
    activity.obbPath = "./oob/";
    activity.sdkVersion = 28;
    activity.vm = vm;
    activity.clazz = nullptr;
    // activity.assetManager = (struct AAssetManager*)23;
    ANativeActivityCallbacks callbacks;
    memset(&callbacks, 0, sizeof(ANativeActivityCallbacks));
    activity.callbacks = &callbacks;
    activity.vm->GetEnv(&(void*&)activity.env, 0);
    auto env = ((JNIEnv*)activity.env);
    jclass clazz = env->FindClass("mcpelauncher/Activity");
    activity.clazz = env->NewObjectA(clazz, env->GetMethodID(clazz, "<init>", "()V"), nullptr);
    auto ANativeActivity_onCreate = (ANativeActivity_createFunc*)linker::dlsym(minecraft, "ANativeActivity_onCreate");
    JNI_OnLoad(vm, nullptr);
    ANativeActivity_onCreate(&activity, nullptr, 0);
    activity.callbacks->onInputQueueCreated(&activity, (AInputQueue *)1);
    activity.callbacks->onStart(&activity);
    // activity.callbacks->onNativeWindowCreated(&activity, (ANativeWindow *)1);
    // activity.callbacks->onResume(&activity);
    std::this_thread::sleep_for(std::chrono::seconds(60));
    // std::mutex deadlock;
    // deadlock.lock();
    // deadlock.lock();
}