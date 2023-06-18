#pragma once
#include <windows.h>
#include <io.h>
#include <process.h>


typedef long long ssize_t;

static inline ssize_t pread64(int fd, void* buf, size_t nbytes, long long offset) {
    OVERLAPPED overlapped;
    memset(&overlapped, 0, sizeof(overlapped));
    overlapped.Pointer = (decltype(overlapped.Pointer))(offset);
    DWORD read;
    return ReadFile((HANDLE)_get_osfhandle(fd), buf, nbytes, &read, &overlapped) ? read : -1;
}

static inline ssize_t pwrite64(int fd, const void* buf, size_t nbytes, long long offset) {
    OVERLAPPED overlapped;
    memset(&overlapped, 0, sizeof(overlapped));
    overlapped.Pointer = (decltype(overlapped.Pointer))(offset);
    DWORD written;
    return WriteFile((HANDLE)_get_osfhandle(fd), buf, nbytes, &written, &overlapped) ? written : -1;
}

#define STDIN_FILENO	0
#define STDOUT_FILENO	1
#define STDERR_FILENO	2

static char *realpath(const char * path, char * resolved_path) {
    HANDLE dest = CreateFileA(path, GENERIC_READ, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
    if(!dest) {
        return nullptr;
    }
    bool malloced = false;
    if(!resolved_path) {
        resolved_path = (char*)malloc(256);
        malloced = true;
    }
    DWORD ret = GetFinalPathNameByHandleA(dest, resolved_path, 255, FILE_NAME_NORMALIZED);
    CloseHandle(dest);
    if(!ret && malloced) {
        free(resolved_path);
    }
    return ret ? resolved_path : nullptr;
}

typedef int pid_t;
typedef int mode_t;

// static pid_t getpid() {
//     return 0;
// }