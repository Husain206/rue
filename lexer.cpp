#include "lexer.h"
#include <cctype>
#include <vector>

bool Lexer::isAtEnd() { return offset >= src.size(); }

char Lexer::peek() { return (isAtEnd()) ? '\0' : src[offset]; }

char Lexer::peekNext() {
  return (offset + 1 >= src.size()) ? '\0' : src[offset + 1];
}

char Lexer::peekPre() {
  return (offset == 0 || isAtEnd()) ? '\0' : src[offset - 1];
}

char Lexer::advance() { return (isAtEnd()) ? '\0' : src[offset++]; }

bool Lexer::match(char c) {
  if (peek() == c) {
    advance();
    return true;
  }
  return false;
}

void Lexer::error(str msg) {
  std::cerr << "main.ru:" << line << ":" << col << ":Lexer Error: " << msg
            << std::endl;
  exit(0);
}

void Lexer::isWhiteSpace() {
  while (!isAtEnd()) {
    if ((peek() == '/' && peekNext() == '/') || peek() == '#') {
      while (peek() != '\n') {
        advance();
      }
    } else if (std::isspace(peek())) {
      advance();
    } else
      break;
  }
}

 Token Lexer::symbolToken(){
  char c = advance();

  switch (c) {
     case '\n': line++;
     case '+': if(match('+')) return {INC, "++", line, col}; else return {PLUS, "+", line, col};
     case '-': if(match('-')) return {DEC, "--", line, col}; else return {MINUS, "-", line, col};
     case '*': return {STAR, "*", line, col};
     case '/': return {SLASH, "/", line, col};
     case ',': return {COMMA, ",", line, col};
     case '.': return {DOT, ".", line, col};
     case ';': return {SEMIC, ";", line, col};
     case '{': return {RCB, "{", line, col};
     case '}': return {LCB, "}", line, col};
     case '(': return {LPRN, "(", line, col};
     case ')': return {RPRN, ")", line, col};
     case '[': return {LB, "[", line, col};
     case ']': return {RB, "]", line, col};
     case '=': if(match('=')) return {EQEQ, "==", line, col}; else return {EQ, "=", line, col};
     case '>': if(match('=')) return {BQ, ">=", line, col};   else return {BGT, ">", line, col};
     case '<': if(match('=')) return {LQ, "<=", line, col};   else return {LST, ">", line, col};
     case '|': if(match('|')) return {OR, "||", line, col};
     case '&': if(match('&')) return {AND, "&&", line, col};  else return {ADDR, "&", line, col};
     case '!': return {NOT, "!", line, col};
     case '"': return string();   
     default: return {INVALID ,"", line, col};   
  }
}


Token Lexer::string() {
  str string;
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n')
      line++;
    string += advance();
  }

  if (isAtEnd()) {
    error("Unterminated string.\n");
  }
  advance();

  return {STRLIT, string, line, col};
}

Token Lexer::setKeyword(str iden) {
  if (iden == "print")
    return {PRINT, "print", line, col};
  else if (iden == "while")
    return {ALA, "while", line, col};
  else if (iden == "if")
    return {IF, "if", line, col};
  else if (iden == "else")
    return {ELSE, "else", line, col};
  else if (iden == "for")
    return {FOR, "for", line, col};
  else if (iden == "set")
    return {SET, "set", line, col};
  else if (iden == "fn")
    return {FN, "fn", line, col};
  else if (iden == "return")
    return {RETURN, "return", line, col};
  else if (iden == "null")
    return {NALL, "null", line, col};
  return {IDENT, iden, line, col};
}

Token Lexer::ID() {
  str iden;
  if(std::isdigit(peekPre())){
    error("cannot start an identifier with a number\n");
    return {INVALID};
  }
  if (std::isalpha(peek())) {
    while (isalnum(peek())) {
      iden += advance();
    } 
  }
  return setKeyword(iden);
}

Token Lexer::Number() {
  str num;
  while (std::isdigit(peek())) {
    num += advance();
  }
  return {NUMLIT, num, line, col};
}

std::vector<Token> Lexer::tokenize(str &source) {
  src = source;
  std::vector<Token> tokens;

  while (true) {
    if (isAtEnd()) {
      tokens.push_back({EoF, "", line, col});
      break;
    }

    // if (peek() == '\n') {
    // col}= 0;
    // line++;
    // } else col}+;

    isWhiteSpace();
    if (std::isalpha(peek())) {
      tokens.push_back(ID());
    } else if (isdigit(peek())) {
      tokens.push_back(Number());
    } else {
      tokens.push_back(symbolToken());
    }
  }
  return tokens;
}
