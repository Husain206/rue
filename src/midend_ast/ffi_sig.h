// backend/ffi_sig.h
#pragma once
#include <string>
#include <vector>
#include <ffi.h>
struct FFIParsed {
  ffi_type* ret = &ffi_type_void;
  std::vector<ffi_type*> fixed;
  bool variadic = false;
  size_t fixed_arity = 0;
};

FFIParsed parse_sig(const std::string& sig); // "i(s,...)"
ffi_type* map_char(char c);                  // i,d,s,p,v
