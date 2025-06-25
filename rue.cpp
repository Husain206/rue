#include "lexer.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

std::string tokenTypeToString(TokenType type) {
  switch (type) {
  case SET:
    return "Set";
  case PRINT:
    return "Print";
  case IDENT:
    return "Ident";
  case NUMLIT:
    return "Num";
  // case VARIABLE:   return "Var";
  case PLUS:
    return "Plus";
  case MINUS:
    return "Minus";
  case STAR:
    return "Mul";
  case SLASH:
    return "Div";
  case EQ:
    return "Equal";
  case SEMIC:
    return "Semi";
  case LPRN:
    return "LParen";
  case RPRN:
    return "RParen";
  case LCB:
    return "LCurly";
  case RCB:
    return "RCurly";
  case EQEQ:
    return "EQEQ";
  case BGT:
    return "BGT";
  case LST:
    return "LST";
  case BQ:
    return "BQ";
  case LQ:
    return "LQ";
  case OR:
    return "OR";
  case AND:
    return "AND";
  case NOT:
    return "NOT";
  case INC:
    return "INC";
  case DEC:
    return "DEC";
  case ADDR:
    return "ADDR";
  case STRLIT:
    return "STRLIT";
  case FN:
    return "fn";
  case RETURN:
    return "return";
  case NALL:
    return "null";
  case EoF:
    return "EOF";
  case INVALID:
    return "Invalid";
  default:
    return "Unknown";
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "usage: [" << argv[0] << "] [file.ru]\n";
    return 1;
  }
  const std::string filename = argv[1];
  std::ifstream file(filename);

  if (!file.is_open()) {
    std::cerr << "Error: Could not open file '" << filename << "'\n";
    return 1;
  }
  std::ostringstream buffer;
  buffer << file.rdbuf();
  str src = buffer.str();

  Lexer lexer;
  std::vector<Token> tokens = lexer.tokenize(src);

  for (const auto &token : tokens) {
    std::cout << "Token(Type: " << tokenTypeToString(token.type)
              << ", Lexeme: \"" << token.lexeme << "\", Line: " << token.line
              << ", Col: " << token.col << ")\n";
  }

  return 0;
}
