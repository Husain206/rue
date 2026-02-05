// backend/ffi_sig.cpp
#include "ffi_sig.h"
#include <stdexcept>

ffi_type* map_char(char c){
  switch(c){
    case 'i': return &ffi_type_sint64;
    case 'd': return &ffi_type_double;
    case 'p': return &ffi_type_pointer;
    case 's': return &ffi_type_pointer; // pass const char*
    case 'v': return &ffi_type_void;
    default: throw std::runtime_error(std::string("unknown ffi type: ")+c);
  }
}

FFIParsed parse_sig(const std::string& sig){
  // grammar:  ret '(' [args] ')' ; args = [type {',' type}] [',...']
  // types are single letters (i,d,s,p). Whitespace ignored.
  size_t i=0, n=sig.size();
  auto skip=[&]{ while(i<n && isspace((unsigned char)sig[i])) ++i; };
  FFIParsed out;

  skip();
  if(i>=n) throw std::runtime_error("empty signature");
  out.ret = map_char(sig[i++]);

  skip();
  if(i>=n || sig[i++]!='(') throw std::runtime_error("expected '('");

  skip();
  if(i<n && sig[i]==')'){ ++i; return out; }

  // parse args until ')' (support ... token)
  while(i<n){
    skip();
    if(sig.compare(i,3,"...")==0){ out.variadic=true; i+=3; break; }
    if(sig[i]==')') break;
    if(sig[i]==','){ ++i; continue; }
    out.fixed.push_back(map_char(sig[i++]));
    out.fixed_arity++;
    skip();
    if(i<n && sig[i]==',') { ++i; }
  }
  if(i>=n || sig[i]!=')') throw std::runtime_error("expected ')'");
  ++i; // consume ')'
  return out;
}
