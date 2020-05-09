#pragma once

#include <unordered_map>
#include <string>
#include <dlfcn.h>

extern "C" {
    void* __loader_dlopen(const char* filename, int flags, const void* caller_addr);
    void* __loader_dlsym(void* handle, const char* symbol, const void* caller_addr);
    int __loader_dladdr(const void* addr, Dl_info* info);
    int __loader_dlclose(void* handle);
    char* __loader_dlerror();
    int __loader_dl_iterate_phdr(int (*cb)(struct dl_phdr_info* info, size_t size, void* data), void* data);
    void __loader_android_update_LD_LIBRARY_PATH(const char* ld_library_path);
}

namespace linker {

    inline void *dlopen(const char* filename, int flags) {
        return __loader_dlopen(filename, flags, nullptr);
    }

    inline void *dlsym(void *handle, const char *symbol) {
        return __loader_dlsym(handle, symbol, nullptr);
    }

    inline int dladdr(const void* addr, Dl_info* info) {
        return __loader_dladdr(addr, info);
    }

    inline int dlclose(void* handle) {
        return __loader_dlclose(handle);
    }

    inline char *dlerror() {
        return __loader_dlerror();
    }

    inline int dl_iterate_phdr(int (*cb)(struct dl_phdr_info* info, size_t size, void *data), void *data) {
        return __loader_dl_iterate_phdr(cb, data);
    }

    inline void update_LD_LIBRARY_PATH(const char *ld_library_path) {
        __loader_android_update_LD_LIBRARY_PATH(ld_library_path);
    }

    void init();

    void *load_library(const char *name, const std::unordered_map<std::string, void*> &symbols);

    size_t get_library_base(void *handle);

    void get_library_code_region(void *handle, size_t &base, size_t &size);

}
