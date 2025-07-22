#include "ir.h"
#include <iostream>
#include <string>

std::vector<IR> ir_program;
static int temp_id = 0;


str new_temp(){
  return "%t" + std::to_string(temp_id++);
}

void emit_const(str dst, str val){
  ir_program.push_back({IR_CONST, dst, val, "", {}});
}

void emit_binop(str dst, str a, str b, IRop op){
  ir_program.push_back({IR_BINOP, dst, a, b, op});
}

void emit_store(str name, str val){
  ir_program.push_back({IR_STORE, "", name, val, {}});
}

void emit_load(str dst, str name){
  ir_program.push_back({IR_LOAD, dst, name, "", {}});
}

void emit_print(str val){
  ir_program.push_back({IR_PRINT, "", val, "", {}});
}


void dump_ir(){
  for(const auto& instrcution : ir_program){
    switch(instrcution.kind){
      case IR_CONST: std::cout << instrcution.dst << " const " << instrcution.operand1 << "\n"; break;
      case IR_BINOP: std::cout << instrcution.dst << " =w "
                               << (instrcution.op == ADD ? "add" :
                                   instrcution.op == SUB ? "sub" :
                                   instrcution.op == MUL ? "mul" : "div")
                               << " " << instrcution.operand1 << ", " << instrcution.operand2 << "\n";
      break;
      case IR_STORE: std::cout << "store " << instrcution.operand1 << ", " << instrcution.operand2 << "\n"; break;
      case IR_LOAD: std::cout << instrcution.dst << " =w load " << instrcution.operand1 << "\n"; break;
      case IR_PRINT: std::cout << "print " << instrcution.operand1 << "\n"; break;
    }
  }
}
