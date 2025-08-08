#pragma once

#include "lexer.h"
#include <vector>


enum prec {
  NONE,
  ASS,
  SUM,
  PRODUCT,
  COMP,
  PREFIX,
  POSTFIX,
};

prec get_prec(TokenType type);

struct Parser {

std::vector<Token> tokens;
usize current = 0;

// recursive decent parser
void parse();
void parse_stmt();
void parse_set();
void parse_print();
void parse_ala();
void parse_if();
void parse_for_loop();
// void parse_return();
// std::vector<Token> parse_params();
// void parse_fn();
// str parse_fn_call(const str& fn_name);


// pratt parser
str parseExpr(int rbp = 0);
int lbp(const Token& t);
str nud(const Token& t);
str led(const Token& t, str left);

// helpers
Token peek();
// Token peekNext();
// Token peekPre();
Token next();

bool isAtEnd();
bool check(TokenType type);
bool match(TokenType type);
void consume(const TokenType& t, str msg);

// check parser
    int indent = 0;
    void printWithIndent(const str& s);

};
