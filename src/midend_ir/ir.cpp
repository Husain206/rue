#include "ir.h"
#include "../frontend/lexer.h"
#include <string>
#include <vector>

void print_indent(int indent=1){
  for (int i{0}; i < indent; i++) {
     cout << "   ";
  }
}

static std::string opToMnemonic(OpKind op) {
  switch (op) {
  case OpKind::ADD:
    return "add";
  case OpKind::SUB:
    return "sub";
  case OpKind::MUL:
    return "mul";
  case OpKind::DIV:
    return "div";
  case OpKind::MOD:
    return "mod";
  case OpKind::LT:
    return "lt";
  case OpKind::GT:
    return "gt";
  case OpKind::LE:
    return "le";
  case OpKind::GE:
    return "ge";
  case OpKind::EQ:
    return "eq";
  case OpKind::EQEQ:
    return "eqeq";
  case OpKind::NEQ:
    return "neq";
  case OpKind::AND:
    return "and";
  case OpKind::OR:
    return "or";
  case OpKind::MOVE:
    return "mov";
  case OpKind::LOAD:
    return "load";
  case OpKind::STORE:
    return "store";
  case OpKind::CALL:
    return "call";
  case OpKind::INC:
    return "inc";
  case OpKind::DEC:
    return "dec";
  default:
    return "???";
  }
}

string IRprog::new_temp() { return "&t" + to_string(++tempCounter); }

string IRprog::new_label(const string &base) {
  return base + to_string(labelCounter);
}

void IRprog::emit(OpKind op, const string &dst, const string &operand1,
                  const string &operand2, const string &label, int argc, const string& fnName, const vector<string>& args) {
  code.push_back({op, dst, operand1, operand2, label, argc, fnName, args});
}

void IRprog::dump() {
  for (auto &i : code) {
    switch (i.op) {
    case OpKind::CALL:
      print_indent();
      cout << "call " << i.operand1 << ", " << i.operand2 << " (argc:" << i.argc
           << ")\n";
      break;

    case OpKind::STORE:
      print_indent();
      cout << "store " << i.dst << ", " << i.operand1 << "\n";
      break;

    case OpKind::LOAD:
      print_indent();
      cout << i.dst << " =w load " << i.operand1 << "\n";
      break;

    case OpKind::LABEL:
      cout << i.label << ":\n";
      break;

    case OpKind::JMP:
      cout << "JMP " << i.label << "\n";
      break;

    case OpKind::JNZ:
      cout << "JNZ " << i.operand1 << ", " << i.label << "\n";
      break;

    case OpKind::RET:
      print_indent();
      cout << "return " << i.operand1 << "\n";
      break;

    case OpKind::FN_BEG:
      print_indent();
      cout << "fn " << i.fnName << "_beg:\n";
      break;
    case OpKind::FN_END:
      print_indent();
      cout << "fn " << i.fnName << "_end\n";
      break;
    case OpKind::PARAMS:
      print_indent();
      cout << "param " << i.operand1 << "\n";
      break;

    default:
      print_indent();
      if (i.op == OpKind::MOVE) {
        cout << i.dst << " =m " << i.operand1 << "\n";
      } else {
        cout << i.dst << " =d " << opToMnemonic(i.op) << " " << i.operand1;
        if (!i.operand2.empty())
          cout << ", " << i.operand2;
        cout << "\n";
      }
    }
  }
}

string IRprog::genExpr(const Node *n) {
  switch (n->type) {
  case n_num:
  case n_str: {
    string t = new_temp();
    emit(OpKind::MOVE, t, n->lexeme);
    return t;
  }

  case n_id: {
    string t = new_temp();
    emit(OpKind::LOAD, t, n->lexeme);
    return t;
  }

  case n_unary: {
    string l = genExpr(n->children[0].get());
    string t = new_temp();

    OpKind op;
    switch (n->op) {
    case INC:
      op = OpKind::INC;
      break;
    case DEC:
      op = OpKind::DEC;
      break;
    default:
      throw std::runtime_error("unhandled unary op");
    }
    emit(op, t, l);
    return t;
  }

  case n_binary: {
    string l = genExpr(n->children[0].get());
    string r = genExpr(n->children[1].get());
    string t = new_temp();
    OpKind op;
    switch (n->op) {
    case PLUS:
      op = OpKind::ADD;
      break;
    case MINUS:
      op = OpKind::SUB;
      break;
    case STAR:
      op = OpKind::MUL;
      break;
    case SLASH:
      op = OpKind::DIV;
      break;
    case EQ:
      op = OpKind::EQ;
      break;
    case MOD:
      op = OpKind::MOD;
      break;
    case NOTEQ:
      op = OpKind::NEQ;
      break;
    case EQEQ:
      op = OpKind::EQEQ;
      break;
    case LST:
      op = OpKind::LT;
      break;
    case GRT:
      op = OpKind::GT;
      break;
    case LQ:
      op = OpKind::LE;
      break;
    case GQ:
      op = OpKind::GE;
      break;
    case OR:
      op = OpKind::OR;
      break;
    case AND:
      op = OpKind::AND;
      break;
    default:
      throw std::runtime_error("unhandled binary op");
    }
    emit(op, t, l, r);
    return t;
  }

  case n_fn_call: {
      vector<string> argTemps;

      for (size_t i = 1; i < n->children.size(); ++i) {
        argTemps.push_back(genExpr(n->children[i].get()));
      }
      string callee;
      if (n->children.size() > 0 && n->children[0]->type == n_id) callee = n->children[0]->lexeme;
      else {
        throw runtime_error("genExpr: unsupported callee expression in fn_call");
      }
      string t = new_temp();
      emit(OpKind::CALL, t, callee, "", "", (int)argTemps.size(), "", argTemps);
      return t;
    }

  
  default:
    throw std::runtime_error("genExpr: unhandled node type " + n->lexeme);
  }
}

void IRprog::genStmt(const Node *n) {
  switch (n->type) {
  case n_set: {
    string rhs = genExpr(n->children[1].get());
    emit(OpKind::STORE, n->children[0]->lexeme, rhs);
    break;
  }
  case n_print: {
    string val = genExpr(n->children[0].get());
    emit(OpKind::CALL, "", "print", val, "", 1);
    break;
  }

  case n_ala: {
    labelCounter++;
    string L1 = new_label("Start");
    string L2 = new_label("body");
    string L3 = new_label("End");

    breakLabels.push_back(L3);
    continueLabels.push_back(L1);

    emit(OpKind::LABEL, "", "", "", L1);
    string cond = genExpr(n->children[0].get());
    emit(OpKind::JNZ, "", cond, "", L2);
    emit(OpKind::JMP, "", "", "", L3);

    emit(OpKind::LABEL, "", "", "", L2);
    genStmt(n->children[1].get());
    emit(OpKind::JMP, "", "", "", L1);

    emit(OpKind::LABEL, "", "", "", L3);

    breakLabels.pop_back();
    continueLabels.pop_back();
    
    break;
  }

  case n_if: {
      labelCounter++;
    string cond = genExpr(n->children[0].get());
    string L1 = new_label("then");
    string L2 = new_label("else");
    string L3 = new_label("end");

    emit(OpKind::JNZ, "", cond, "", L1);
    emit(OpKind::JMP, "", "", "", L2);

    emit(OpKind::LABEL, "", "", "", L1);
    genStmt(n->children[1].get());
    emit(OpKind::JMP, "", "", "", L3);

    emit(OpKind::LABEL, "", "", "", L2);
    if (n->children.size() > 2)
      genStmt(n->children[2].get());
    emit(OpKind::LABEL, "", "", "", L3);
    break;
  }

  case n_for_loop: {
    labelCounter++;
    string L1 = new_label("start");
    string L2 = new_label("body");
    string L3 = new_label("end");

    genStmt(n->children[0].get());

    emit(OpKind::LABEL, "", "", "", L1);
    string cond = genExpr(n->children[1].get());
    emit(OpKind::JNZ, "", cond, "", L2);
    emit(OpKind::JMP, "", "", "", L3);

    emit(OpKind::LABEL, "", "", "", L2);
    genStmt(n->children[3].get());
    genExpr(n->children[2].get());
    emit(OpKind::JMP, "", "", "", L1);

    emit(OpKind::LABEL, "", "", "", L3);

    break;
  }

  case n_break: {
      if (breakLabels.empty()) throw runtime_error("break used outside loop");
      emit(OpKind::JMP, "", "", "", breakLabels.back());
      break;
    }

    case n_continue: {
      if (continueLabels.empty()) throw runtime_error("continue used outside loop");
      emit(OpKind::JMP, "", "", "", continueLabels.back());
      break;
    }

  case n_fn_dec: {
      if (n->children.empty()) throw runtime_error("fn_dec: missing body");

      string fname = n->lexeme;
      emit(OpKind::FN_BEG, "", "", "", "", 0, fname);

      for (size_t i = 0; i + 1 < n->children.size(); ++i) {
        const Node* p = n->children[i].get();
        if (p->type != n_id) throw runtime_error("fn_dec: parameter must be identifier");
        emit(OpKind::PARAMS, "", p->lexeme); 
      }

      genStmt(n->children.back().get());

      emit(OpKind::FN_END, "", "", "", "", 0, fname);
      break;
    }

    case n_return: {
      if (n->children.empty()) {
        emit(OpKind::RET, "", "");
      } else {
        string val = genExpr(n->children[0].get());
        emit(OpKind::RET, "", val);
      }
      break;
    }

    case n_fn_call: {
      genExpr(n);
      break;
    }

  case n_else:
  case n_block: {
    for (auto &c : n->children)
      genStmt(c.get());
    break;
  }

  default:
    throw std::runtime_error("genStmt: unhandled node type " + n->lexeme);
  }
}
