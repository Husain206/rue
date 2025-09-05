#include "../frontend/ast.h"
#include "../frontend/lexer.h"
#include "../frontend/parser.h"
#include "ir.h"
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>

int main(int argc, char *argv[]) {

  if (argc < 2) {
    std::cerr << "usage: [" << argv[0] << "] [file.ru]\n";
    return 1;
  }
  std::string fname = argv[1];
if (fname.size() < 3 || fname.substr(fname.size() - 3) != ".ru") {
    std::cerr << "file extension must end with '.ru'\n";
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
  auto tokens = lexer.tokenize(src);

  Parser parser;
  parser.tokens = tokens;
  auto ast = parser.parse();
  // printAST(ast.get());
  IRprog ir;
  ir.genStmt(ast.get());
  ir.dump();
  // VM vm;
  // vm.run(ir);
  
  return 0;
}

