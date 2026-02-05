// backend/ffi.h
#pragma once
#include <string>
#if defined(_WIN32)
  #include <windows.h>
  using LibHandle = HMODULE;
#else
  #include <dlfcn.h>
#endif

void* ffi_open(const std::string& path);
void*     ffi_symbol(void* h, const char* name);
void      ffi_close(void* h);
std::string libc_default();  // pick libc name per OS

