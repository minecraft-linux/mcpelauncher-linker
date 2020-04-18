#ifdef __BIONIC__
#undef __BIONIC__
#endif
#ifdef __linux__
#include_next <sys/auxv.h>
#endif