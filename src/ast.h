#pragma once

#include "lexer.h"
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
      n_ala,
      n_if,
      n_else,
      n_for_loop,
      n_block,
      n_expr_stmt,
} NodeType;


struct Node {
   
   NodeType type;
   string lexeme;
   TokenType op = EoF;

   vector<unique_ptr<Node>> children;

   Node(NodeType k) : type(k) {}
};

void printAST(const Node* node, int indent = 0);
