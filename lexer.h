#pragma once

#include <iostream>
#include <string>
#include <vector>

typedef std::string str;
typedef size_t usize;

typedef enum {

  // keywords
  ALA = 0,
  FOR = 1,
  IF = 2,
  ELSE = 3,
  PRINT = 4,
  SET = 5,
  FN = 6,
  RETURN = 7,
  NALL = 8,

  // IDent
  IDENT = 9,
  NUMLIT = 10,
  STRLIT = 11,
  VAR = 12,

  // binary op
  PLUS = 13,
  MINUS = 14,
  STAR = 15,
  SLASH = 16,
  EQ = 17,

  // symbols
  RCB = 18,
  LCB = 19, // { }
  RPRN = 20,
  LPRN = 21, // ( )
  RB = 22,
  LB = 23, // []
  SEMIC = 24,
  COMMA = 25,
  DOT = 26,
  DQ = 27,
  SQ = 28,

  // comparsion
  NOTEQ = 29,
  EQEQ = 30,
  LQ = 31,
  BQ = 32,
  LST = 33,
  BGT = 34,

  // logical operators
  OR = 35,
  AND = 36,
  NOT = 37,

  // unary operators
  ADDR = 38,
  DEREF = 39,
  INC = 40,
  DEC = 41,

  // validation
  INVALID = 42,
  EoF = 43
} TokenType;

typedef struct {

  TokenType type;
  str lexeme;
  usize line, col;

} Token;

struct Lexer {

  str src;
  std::vector<Token> tokenize(const str &source);
  usize offset = 0, line = 1, col = 0;

  void isWhiteSpace();

  Token makeToken(TokenType type, str lexeme, usize line, usize col);
  Token symbolToken();
  Token toString();
  Token setKeyword(str id);
  Token ID();
  Token Number();

  void error(str msg);
  
  // helpers
  char peek();
  char peekNext();
  char peekPre();
  char advance();
  char next();
  bool match(char c);
  bool isAtEnd();

};
