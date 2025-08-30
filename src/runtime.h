#include "ast.h"
#include <ostream>
#include "sym_table.h"


struct ReturnException {
  Value value;
  ReturnException(Value v) : value(v) {}
};

struct Interpreter {
  void run(const Node* root);

  Env env;
  // ostream* out = &cout;
  void exec(const Node* n);
  void exec_block(const Node* block);
  void exec_set(const Node* n);
  void exec_print(const Node* n);
  // void exec_input(const Node* n);
  void exec_ala(const Node* n);
  void exec_if(const Node* n);
  void exec_for_loop(const Node* n);
  void exec_fn(const Node* n);
  void exec_return(const Node* n);
  
  Value eval(const Node* n);
  Value eval_id(const Node* n);
  Value eval_assign(const Node* n);
  Value eval_binary(const Node* n);
  Value eval_unary(const Node* n);
  Value eval_call(const Node* n);

  Value coerceInt(const Value& v);
  Value plus(const Value& a, const Value& b);
  bool equals(const Value& a, const Value& b);
  
  };
