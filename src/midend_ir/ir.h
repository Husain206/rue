#pragma once

#include <string>
#include <vector>
#include "../frontend/ast.h"

using namespace std;

enum struct OpKind {
  ADD,
  SUB,
  MUL,
  DIV,
  MOD,
  LT,
  GT,
  LE,
  GE,
  EQ,
  EQEQ,
  NEQ,
  AND,
  OR,

  INC,
  DEC,
  
  LOAD,
  STORE,
  MOVE,
  CALL,

  JMP,
  JNZ,
  LABEL,

  RET,
  PARAMS,
  FN_BEG,
  FN_END,
};

struct IR {
   OpKind op;
   string dst, operand1, operand2, label;
   int argc = 0;
   string fnName;
   vector<string> args;
};


struct IRprog {
   vector<IR> code;
   int tempCounter = 0;
   int labelCounter = 0;

   vector<string> breakLabels;
   vector<string> continueLabels;

   string new_temp();
   string new_label(const string& base="L");
   void emit(OpKind op,
             const string& dst="",
             const string& operand1="",
             const string& operand2="",
             const string& label="",
             int argc = 0,
             const string& fnName="",
             const vector<string>& args={});

   void dump();
   string genExpr(const Node* n);
   void genStmt(const Node* n);
};
