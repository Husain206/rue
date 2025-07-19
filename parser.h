#pragma once

#include "lexer.h"
#include <vector>

struct Parser {

std::vector<Token> tokens;
usize current = 0;

str parseExpr(int rbp = 0);
int lbp(const Token& t);
str nud(const Token& t);

// helpers
Token peek();
Token peekNext();
Token peekPre();
Token next();

bool isAtEnd();
bool match(TokenType type);

};
