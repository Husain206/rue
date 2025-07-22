#pragma once

#include <string>
#include <vector>

typedef std::string str;

enum IRkind {
  IR_CONST,
  IR_BINOP,
  IR_STORE,
  IR_LOAD,
  IR_PRINT,
  IR_LABEL,
  IR_JMP,
  IR_JZ
};

enum IRop {
  ADD,
  SUB,
  MUL,
  DIV
};


struct IR {
  IRkind kind;
  str dst;
  str operand1, operand2;
  IRop op;
  str label;
};

str new_temp();
void emit_const(str dst, str val);
void emit_binop(str dst, str a, str b, IRop op);
void emit_store(str name, str val);
void emit_load(str dst, str name);
void emit_print(str val);
void dump_ir();


extern std::vector<IR> ir_program; 
