#include "ffi.h"
#include <stdexcept>

void* ffi_open(const std::string& path) {
#if defined(_WIN32)
  HMODULE h = LoadLibraryA(path.c_str());
  if(!h) throw std::runtime_error("LoadLibrary failed: " + path);
  return h;
#else
  dlerror();
  void* h = dlopen(path.c_str(), RTLD_LAZY);
  const char* e = dlerror();
  if(!h || e) throw std::runtime_error(std::string("dlopen failed: ") + (e?e:""));
  return h;
#endif
}

void* ffi_symbol(void* h, const char* name) {
#if defined(_WIN32)
  void* p = (void*)GetProcAddress(h, name);
  if(!p) throw std::runtime_error(std::string("GetProcAddress failed: ") + name);
  return p;
#else
  dlerror();
  void* p = dlsym(h, name);
  const char* e = dlerror();
  if(e) throw std::runtime_error(std::string("dlsym failed: ") + e);
  return p;
#endif
}

void ffi_close(void* h) {
#if defined(_WIN32)
  if(h) FreeLibrary(h);
#else
  if(h) dlclose(h);
#endif
}

std::string libc_default() {
#if defined(_WIN32)
  return "msvcrt.dll";
#elif defined(__APPLE__)
  return "/usr/lib/libSystem.B.dylib";
#else
  return "libc.so.6";
#endif
}
