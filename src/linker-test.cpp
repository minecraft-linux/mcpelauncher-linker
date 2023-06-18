typedef long long off64_t;

#include <fcntl.h>
#include <mcpelauncher/linker.h>

__attribute__((sysv_abi)) static void __abort() {
    abort();
}

int main() {
    _set_fmode(_O_BINARY);
    linker::init();
    std::unordered_map<std::string, void *> syms;
    syms["__cxa_finalize"] = reinterpret_cast<void*>(&__abort);
    syms["__cxa_atexit"] = reinterpret_cast<void*>(&__abort);
    linker::load_library("libc.so", syms);
    auto handle = linker::dlopen(".\\test.so", 0);
    if(!handle) {
        printf("linker::dlerror %s\n", linker::dlerror());
        return 1;
    }
}