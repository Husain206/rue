#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "lexer.h"

#define RESET        "\033[0m"
#define BLUE         "\033[34m"
#define GREEN        "\033[32m"
#define YELLOW       "\033[33m"
#define DARKGREEN    "\033[36m"
#define RED          "\033[31m"
#define BLACK        "\033[31m"

std::string tokenTypeToString(TokenType type) {
  switch (type) {
  case SET:
    return "Set";
  case PRINT:
    return "print";
  case IDENT:
    return "Ident";
  case NUMLIT:
    return "Num";
  // case VARIABLE:   return "Var";
  case IF:
    return "if";
  case ALA:
    return "ala";
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

  std::cout << std::endl << "the code: ";
  for (const auto &token : tokens) {
    std::cout << BLACK << token.lexeme << RESET << " ";
  }
  std::cout << std::endl << std::endl;
   
  for (const auto &token : tokens) {
        std::cout << "  \033[35mType:\033[0m "
        << RED << tokenTypeToString(token.type) << RESET
        << ", " << GREEN << "Lexeme: " << "\"" << token.lexeme << "\"" << RESET
        << ", " << BLUE << "Line: " << token.line << RESET
        << ", " << DARKGREEN << "Col: " << token.col << RESET << "\n";
    
  }
  return 0;
}
