#pragma once

#include <iostream>
#include <string>
#include <vector>

typedef std::string str;
typedef size_t usize;

typedef enum {

  // keywords
  ALA,
  FOR,
  IF,
  ELSE,
  PRINT,
  SET,
  FN,
  RETURN,
  NALL,

  // IDent
  IDENT,
  NUMLIT,
  STRLIT,
  ASCII_CH,
  VAR,

  // binary op
  PLUS,
  MINUS,
  STAR,
  SLASH,
  EQ,

  // symbols
  RCB,
  LCB, // { }
  RPRN,
  LPRN, // ( )
  RB,
  LB, // []
  SEMIC,
  COMMA,
  DOT,
  DQ,
  SQ,

  // comparsion
  NOTEQ,
  EQEQ,
  LQ,
  GQ,
  LST,
  GRT,

  // logical operators
  OR,
  AND,
  NOT,
  MOD,

  BITWISE_OR,
  BITWISE_XOR,
  BITWISE_NOT,
  BITWISE_LEFT_SHIFT,
  BITWISE_RIGHT_SHIFT,

  // unary operators
  ADDR,
  DEREF,
  INC,
  DEC,

  // validation
  INVALID,
  EoF,
} TokenType;

typedef struct {

  TokenType type;
  str lexeme;
  usize line, col;

} Token;

struct Lexer {

  str src;
  std::vector<Token> tokenize(const str &source);
  std::vector<int> bracketDeep;
  usize offset = 0, line = 1, col = 0;

  void isWhiteSpace();

  Token makeToken(TokenType type, str lexeme, usize line, usize col);
  Token symbolToken();
  Token toString();
  Token toAscii();
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
