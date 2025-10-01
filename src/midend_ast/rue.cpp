#include "../frontend/lexer.h"
#include "../frontend/parser.h"
#include "../frontend/ast.h"
#include "runtime.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int main(int argc, char *argv[]) {

  if (argc < 2) {
    std::cerr << "usage: [" << argv[0] << "] [file.rue]\n";
    return 1;
  }
  std::string fname = argv[1];
if (fname.size() < 3 || fname.substr(fname.size() - 4) != ".rue") {
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
  vector<string> args(argv + 1, argv + argc);

  Lexer lexer;
  auto tokens = lexer.tokenize(src);

  Parser parser;
  parser.tokens = tokens;
  auto ast = parser.parse();
  // printAST(ast.get());
  Interpreter inter;
  try{
    inter.run(ast.get(), args);
  } catch(const exception& e){
    cerr << "runtime error: " << e.what() << endl; 
  }
  
  
  return 0;
}

