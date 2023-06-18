typedef long long off64_t;

#include <mcpelauncher/linker.h>

int main() {
    linker::init();
    auto handle = linker::dlopen(".\\test.so", 0);
    if(!handle) {
        printf("linker::dlerror %s\n", linker::dlerror());
        return 1;
    }
}