typedef long long off64_t;

#include <fcntl.h>
#include <mcpelauncher/linker.h>

__attribute__((sysv_abi)) static void __abort() {
    abort();
}

__attribute__((sysv_abi)) void print(const char* message) {
    printf("%s\n", message);
}

int main() {
    _set_fmode(_O_BINARY);
    linker::init();
    std::unordered_map<std::string, void *> syms;
    syms["__cxa_finalize"] = reinterpret_cast<void*>(&__abort);
    syms["__cxa_atexit"] = reinterpret_cast<void*>(&__abort);
    syms["print"] = reinterpret_cast<void*>(&print);
    syms["__stack_chk_fail"] = reinterpret_cast<void*>(&__abort);
    linker::load_library("libc.so", syms);
    auto handle = linker::dlopen(".\\test.so", 0);
    if(!handle) {
        printf("linker::dlerror %s\n", linker::dlerror());
        return 1;
    }
    auto entrypoint = (int(*)())linker::dlsym(handle, "entrypoint");

    auto code = entrypoint();
}