
#pragma once

#include "lexer.h"
#include <memory>
#include <vector>
#include "ast.h"


enum prec {
  NONE,
  ASS,
  COMP,
  SUM,
  PRODUCT,
  PREFIX,
  CALL,
  POSTFIX,
};

prec get_prec(TokenType type);

struct Parser {

std::vector<Token> tokens;
usize current = 0;

// recursive decent parser
unique_ptr<Node> parse();
unique_ptr<Node> parse_stmt();
unique_ptr<Node> parse_set();
unique_ptr<Node> parse_print();
unique_ptr<Node> parse_input();
unique_ptr<Node> parse_ala();
unique_ptr<Node> parse_if();
unique_ptr<Node> parse_for_loop();
unique_ptr<Node> parse_break();
unique_ptr<Node> parse_continue();
unique_ptr<Node> parse_return();
unique_ptr<Node> parse_fn();


// pratt parser
unique_ptr<Node> parseExpr(int rbp = 0);
int lbp(const Token& t);
unique_ptr<Node> nud(const Token& t);
unique_ptr<Node> led(const Token& t, unique_ptr<Node> left);

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
