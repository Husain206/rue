#include "parser.h"
#include "lexer.h"
#include <string>
#include <stdexcept>

bool Parser::isAtEnd() { return current >= tokens.size(); }

Token Parser::peek() { return (!isAtEnd()) ? tokens[current] : throw std::runtime_error("unexpcted EOF input"); }

Token Parser::peekNext() { return (current + 1 >= tokens.size()) ? throw std::runtime_error("unexpcted EOF input") : tokens[current + 1];}

Token Parser::peekPre() { return tokens[current - 1]; }

Token Parser::next() { return (!isAtEnd()) ? tokens[current++] : throw std::runtime_error("unexpcted EOF input"); }

bool Parser::match(TokenType type){
  if (peek().type == type) {
    next();
    return true;
  }
  return false;
}

int Parser::lbp(const Token& t){
  switch (t.type) {
   case PLUS:
   case MINUS: return 10; break;
   case STAR:
   case SLASH: return 20; break;
   case LPRN: return  100; break;
   default: return 0;   
  }
}

str Parser::nud(const Token& t){
    switch (t.type) {
      case NUMLIT: return t.lexeme;
      case LPRN: {
      str inner = parseExpr();
      if (peek().type != RPRN) std::cerr << "expected ')' at " << t.line  << ":" << t.col << std::endl;
      else next();
      return inner;
    }
      default: std::cerr << "unexpected token: " << t.lexeme << ":" << t.line << ":" << t.col << std::endl;
      return 0; 
    }
}

  str Parser::parseExpr(int rbp){ // rbp = 0
  Token token = next();
  str left = nud(token);
  while(!isAtEnd() && lbp(peek()) > rbp){
   
   Token op = next();
   str right = parseExpr(lbp(op));
   left = "( " + left + " " + op.lexeme + " " + right + " )";
   
    }
    return left;
  }

