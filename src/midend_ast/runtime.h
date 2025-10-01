#include "../frontend/ast.h"
#include <ostream>
#include <vector>
#include "sym_table.h"


struct ReturnException {
  Value value;
  ReturnException(Value v) : value(v) {}
};

struct Interpreter {
  Value callFunction(const Value& fnVal, const std::vector<Value>& args);
  void run(const Node* root, const std::vector<std::string>& args = {});

  Env env;
  void exec(const Node* n);
  void exec_block(const Node* block, bool declareOnly=false);
  void exec_set(const Node* n);
  void exec_print(const Node* n);
  void exec_input(const Node* n);
  void exec_ala(const Node* n);
  void exec_if(const Node* n);
  void exec_for_loop(const Node* n);
  void exec_fn(const Node* n);
  void exec_return(const Node* n);
  void exec_struct(const Node* n);
  
  Value eval(const Node* n);
  Value eval_id(const Node* n);
  Value eval_assign(const Node* n);
  Value eval_binary(const Node* n);
  Value eval_unary(const Node* n);
  Value eval_call(const Node* n);
  Value eval_ternary(const Node* n);
  Value eval_array(const Node* n);
  Value eval_index(const Node* n);
  Value eval_struct_init(const Node* n);
  Value eval_struct_index(const Node* n);
  
  static Value coerceInt(const Value& v);
  Value plus(const Value& a, const Value& b);
  bool equals(const Value& a, const Value& b);
};
