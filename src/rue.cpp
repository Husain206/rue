#include "ast.h"
#include "runtime.h"
#include "lexer.h"
#include "parser.h"
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>


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
  auto tokens = lexer.tokenize(src);

  Parser parser;
  parser.tokens = tokens;
  auto ast = parser.parse();
  // printAST(ast.get());
  Interpreter inter;
  try{
    inter.run(ast.get());
  } catch(const exception& e){
    cerr << "runtime error: " << e.what() << endl; 
  }
  
  return 0;
}

