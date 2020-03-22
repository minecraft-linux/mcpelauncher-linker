#include <cstring>
extern "C" void* __loader_dlopen(const char* filename, int flags, const void* caller_addr);
extern "C" void* __loader_dlsym(void* handle, const char* symbol, const void* caller_addr);

int main(int argc, char** argv) {
    void * libc = __loader_dlopen("/home/christopher/Downloads/android-ndk-r20/platforms/android-29/arch-x86_64/usr/lib64/libc.so", 0, 0);
    void * libdl = __loader_dlopen("/home/christopher/Downloads/android-ndk-r20/platforms/android-29/arch-x86_64/usr/lib64/libdl.so", 0, 0);
    void * libcpp = __loader_dlopen("/home/christopher/Downloads/android-ndk-r20/sources/cxx-stl/llvm-libc++/libs/x86_64/libc++_shared.so", 0, 0);
    // auto sym = (void*(*)(void*, int, size_t))__loader_dlsym(lib, "memset", nullptr);
    // char buf[10];
    // auto res = sym(buf, 0, 10);
    // if(buf[0]) {
    //     throw 34;
    // }
    return 0;
}