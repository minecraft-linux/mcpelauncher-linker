#include "hooks_darwin_pthread_once.h"

#include <pthread.h>
#include <mutex>
#include <atomic>

std::recursive_mutex _pthread_once_mutex;

extern "C" int darwin_my_pthread_once(int *once_control, void (*init_routine)()) {
    std::unique_lock<std::recursive_mutex> lock (_pthread_once_mutex);
    if (*once_control == 0) {
        *once_control = 1;
        printf("pthread_once -> calling %p\n", (void*) init_routine);
        init_routine();
    }
    return 0;
}

// void* _pthread_once_done;
// std::mutex _pthread_once_mutex;

// extern "C" int darwin_my_pthread_once(void *once_control, void (*init_routine)(void)) {
//     std::atomic<void*>* atm = (std::atomic<void*>*) once_control;
//     std::unique_lock<std::mutex> lock (_pthread_once_mutex);
//     void* ldval = atm->load();
//     if (ldval == (void*) &_pthread_once_done)
//         return 0;

//     if (ldval == nullptr) {
//         std::shared_ptr<pthread_once_t>* once_control_r = new std::shared_ptr<pthread_once_t>(new pthread_once_t());
//         **once_control_r = PTHREAD_ONCE_INIT;
//         atm->store(once_control_r);
//         lock.unlock();
//         printf("pthread_once with init_routine %llx", (long long)init_routine);
//         pthread_once((*once_control_r).get(), init_routine);
//         lock.lock();
//         atm->store((void*) &_pthread_once_done);
//         delete once_control_r; // this will delete the pointer as well after all references are gone
//     } else {
//         std::shared_ptr<pthread_once_t> once_control_r = *((std::shared_ptr<pthread_once_t>*) ldval); // deref it so we get a copy of the pointer and hold a ref
//         lock.unlock();
//         pthread_once(once_control_r.get(), init_routine);
//     }
//     return 0;
// }
