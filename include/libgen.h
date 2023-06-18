#pragma once
#include <string.h>
#define MAXPATHLEN 255

static char basename_buf[MAXPATHLEN + 1];
static char dirname_buf[MAXPATHLEN + 1];

static int __basename_r(const char* path, char* buffer, size_t buffer_size) {
  const char* startp = nullptr;
  const char* endp = nullptr;
  int len;
  int result;

  // Empty or NULL string gets treated as ".".
  if (path == nullptr || *path == '\0') {
    startp = ".";
    len = 1;
    goto Exit;
  }

  // Strip trailing slashes.
  endp = path + strlen(path) - 1;
  while (endp > path && *endp == '/') {
    endp--;
  }

  // All slashes becomes "/".
  if (endp == path && *endp == '/') {
    startp = "/";
    len = 1;
    goto Exit;
  }

  // Find the start of the base.
  startp = endp;
  while (startp > path && *(startp - 1) != '/') {
    startp--;
  }

  len = endp - startp +1;

 Exit:
  result = len;
  if (buffer == nullptr) {
    return result;
  }
  if (len > static_cast<int>(buffer_size) - 1) {
    len = buffer_size - 1;
    result = -1;
    errno = ERANGE;
  }

  if (len >= 0) {
    memcpy(buffer, startp, len);
    buffer[len] = 0;
  }
  return result;
}

static int __dirname_r(const char* path, char* buffer, size_t buffer_size) {
  const char* endp = nullptr;
  int len;
  int result;

  // Empty or NULL string gets treated as ".".
  if (path == nullptr || *path == '\0') {
    path = ".";
    len = 1;
    goto Exit;
  }

  // Strip trailing slashes.
  endp = path + strlen(path) - 1;
  while (endp > path && *endp == '/') {
    endp--;
  }

  // Find the start of the dir.
  while (endp > path && *endp != '/') {
    endp--;
  }

  // Either the dir is "/" or there are no slashes.
  if (endp == path) {
    path = (*endp == '/') ? "/" : ".";
    len = 1;
    goto Exit;
  }

  do {
    endp--;
  } while (endp > path && *endp == '/');

  len = endp - path + 1;

 Exit:
  result = len;
  if (len + 1 > MAXPATHLEN) {
    errno = ENAMETOOLONG;
    return -1;
  }
  if (buffer == nullptr) {
    return result;
  }

  if (len > static_cast<int>(buffer_size) - 1) {
    len = buffer_size - 1;
    result = -1;
    errno = ERANGE;
  }

  if (len >= 0) {
    memcpy(buffer, path, len);
    buffer[len] = 0;
  }
  return result;
}

#undef MAXPATHLEN

static char* basename(const char* path) {
  char* buf = basename_buf;
  int rc = __basename_r(path, buf, sizeof(basename_buf));
  return (rc < 0) ? nullptr : buf;
}

#ifndef NO_DIRNAME
static char* dirname(const char* path) {
  char* buf = dirname_buf;
  int rc = __dirname_r(path, buf, sizeof(dirname_buf));
  return (rc < 0) ? nullptr : buf;
}
#endif