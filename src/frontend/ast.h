#pragma once

#include "../frontend/lexer.h"
#include <memory>
#include <string>
#include <vector>

using namespace std;

typedef enum {
      n_num,
      n_str,
      n_id,
      n_unary,
      n_binary,
      n_assign,
      n_set,
      n_print,
      n_input,
      n_ala,
      n_if,
      n_else,
      n_for_loop,
      n_block,
      n_expr_stmt,
      n_break,
      n_continue,
      n_fn_dec,
      n_fn_call,
      n_return,
      n_ternary,
} NodeType;


struct Node {
   
   NodeType type;
   string lexeme;
   TokenType op = EoF;

   vector<unique_ptr<Node>> children;

   Node(NodeType k) : type(k) {}
};

void printAST(const Node* node, int indent = 0);
