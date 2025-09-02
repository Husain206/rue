#include "../frontend/lexer.h"
#include "../frontend/parser.h"
#include "../frontend/ast.h"
#include "runtime.h"
#include <fstream>
#include <iostream>
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



//   if (argc < 2) {
//     Lexer lexer;
//     Parser parser;
//     Interpreter inter;
//     std::string input;
//     while (true) {
//         std::cout << ">> ";
//         if (!std::getline(std::cin, input) || input == "exit") break;
//         auto tokens = lexer.tokenize(input);
//         parser.tokens = tokens;
//         auto expr = parser.parseExpr();
//         if (!expr) continue;
//         auto node = std::make_unique<Node>(n_expr_stmt);
//         node->children.push_back(std::move(expr));
//         printAST(node.get());
//         // try {
//         //     inter.eval(expr.get());
//         // } catch (const std::exception& e) {
//         //     std::cerr << "runtime error: " << e.what() << std::endl;
//         // }
//     }
//     return 0;
// }

    
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


