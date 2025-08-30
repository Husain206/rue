#include "ast.h"
#include <iostream>

void printIndent(int indent){
  for(int i{0}; i < indent; i++)
    cout << "   ";
}

void printAST(const Node *node, int indent){
  if(!node) return;
  printIndent(indent);
  switch (node->type) {
    case n_num:
      std::cout << "Num(" << node->lexeme << ")\n"; break;
    case n_str:
      std::cout << "Str(\"" << node->lexeme << "\")\n"; break;
    case n_id:
      std::cout << "Ident(" << node->lexeme << ")\n"; break;
    case n_unary:
      std::cout << "Unary(" << node->lexeme << ")\n";
      if (!node->children.empty()) printAST(node->children[0].get(), indent + 1);
      break;
    case n_binary:
      std::cout << "Binary(" << node->lexeme << ")\n";
      for (auto &c : node->children) printAST(c.get(), indent + 1);
      break;
    case n_assign:
      std::cout << "Assign(=)\n";
      for (auto &c : node->children) printAST(c.get(), indent + 1);
      break;
    case n_set:
      std::cout << "SetStmt\n";
      for (auto &c : node->children) printAST(c.get(), indent + 1);
      break;
    case n_print:
      std::cout << "PrintStmt\n";
      for (auto &c : node->children) printAST(c.get(), indent + 1);
      break;
    case n_input:
      std::cout << "InputStmt\n";
      for (auto &c : node->children) printAST(c.get(), indent + 1);
      break;
    case n_ala:
      std::cout << "alaStmt\n";
      printIndent(indent+1);
      std::cout << "Cond\n";
      for(auto &c : node->children) printAST(c.get(), indent + 2);
      break;
    case n_break:
      std::cout << "BreakStmt\n";
      break;
    case n_continue:
      std::cout << "ContinueStmt\n";
      break;
    case n_if:
      std::cout << "IfStmt\n";
      printIndent(indent+1);
      std::cout << "Cond\n";
      for (auto &c : node->children) printAST(c.get(), indent + 2);
      break;
    case n_else:
      std::cout << "Else\n";
      for (auto &c : node->children) printAST(c.get(), indent + 1);
      break;
    case n_for_loop:
      std::cout << "ForStmt\n";
      printIndent(indent+1);
      std::cout << "Cond\n";
      for (auto &c : node->children) printAST(c.get(), indent + 2);
      break;
    case n_block:
      std::cout << "Block\n";
      for (auto &c : node->children) printAST(c.get(), indent + 1);
      break;
    case n_expr_stmt:
      std::cout << "ExprStmt\n";
      for (auto &c : node->children) printAST(c.get(), indent + 1);
      break;
    case n_fn_dec:
      std::cout << "FnDecl(" << node->lexeme << ")\n";
      for (auto &c : node->children) printAST(c.get(), indent + 1);
      break;
    case n_return:
      std::cout << "ReturnStmt\n";
      if (!node->children.empty()) printAST(node->children[0].get(), indent + 1);
      break;
    case n_fn_call:
      std::cout << "Call(" << node->lexeme << ")\n";
      for (auto &c : node->children) printAST(c.get(), indent + 1);
      break;
  }
}

