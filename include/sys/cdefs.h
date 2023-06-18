#pragma once
#ifndef __APPLE__
#include "../../bionic/libc/include/sys/cdefs.h"
#endif
#ifndef _WIN32
#include_next <sys/cdefs.h>
#endif
#ifdef __BIONIC__
#undef __BIONIC__
#endif