#include "../frontend/lexer.h"
#include "../frontend/parser.h"
#include "../frontend/ast.h"
#include "runtime.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

typedef std::string str;

static bool g_debug_lexer = false;

static const char* tokenTypeToString(TokenType t) {
  switch (t) {
    // keywords
    case ALA:        return "ALA";
    case FOR:        return "FOR";
    case IF:         return "IF";
    case ELSE:       return "ELSE";
    case PRINT:      return "PRINT";
    case INPUT:      return "INPUT";
    case SET:        return "SET";
    case FN:         return "FN";
    case RETURN:     return "RETURN";
    case NALL:       return "NALL";
    case CONTINUE:   return "CONTINUE";
    case BREAK:      return "BREAK";
    case STRUCT:     return "STRUCT";
    case ENUM:       return "ENUM";
    case EXTERN:     return "EXTERN";
    case IMPORT:     return "import";

    // literals
    case IDENT:      return "IDENT";
    case NUMLIT:     return "NUMLIT";
    case STRLIT:     return "STRLIT";
    case ASCII_CH:   return "ASCII_CH";

    // binary op
    case PLUS:       return "PLUS";
    case MINUS:      return "MINUS";
    case STAR:       return "STAR";
    case SLASH:      return "SLASH";
    case EQ:         return "EQ";
    case PLUSEQ:     return "PLUSEQ";
    case MINUSEQ:    return "MINUSEQ";
    case MULEQ:      return "MULEQ";
    case DIVEQ:      return "DIVEQ";
    case MODEQ:      return "MODEQ";

    // symbols
    case RCB:        return "RCB";
    case LCB:        return "LCB";
    case RPRN:       return "RPRN";
    case LPRN:       return "LPRN";
    case RB:         return "RB";
    case LB:         return "LB";
    case SEMIC:      return "SEMIC";
    case COMMA:      return "COMMA";
    case DOT:        return "DOT";
    case COLON:      return "COLON";
    case DQ:         return "DQ";
    case SQ:         return "SQ";
    case POW:        return "POW";

    // comparison
    case NOTEQ:      return "NOTEQ";
    case EQEQ:       return "EQEQ";
    case LQ:         return "LQ";
    case GQ:         return "GQ";
    case LST:        return "LST";
    case GRT:        return "GRT";

    // logical operators
    case OR:         return "OR";
    case AND:        return "AND";
    case NOT:        return "NOT";
    case MOD:        return "MOD";
    case QUES:       return "QUES";

    // bitwise
    case BITWISE_OR:          return "BITWISE_OR";
    case BITWISE_XOR:         return "BITWISE_XOR";
    case BITWISE_NOT:         return "BITWISE_NOT";
    case BITWISE_LEFT_SHIFT:  return "BITWISE_LEFT_SHIFT";
    case BITWISE_RIGHT_SHIFT: return "BITWISE_RIGHT_SHIFT";

    // unary operators
    case ADDR:       return "ADDR";
    case DEREF:      return "DEREF";
    case INC:        return "INC";
    case DEC:        return "DEC";

    // validation
    case INVALID:    return "INVALID";
    case EoF:        return "EoF";

    default:         return "UNKNOWN_TOKEN";
  }
}

static std::string dirname(const std::string& path) {
  auto pos = path.find_last_of("/\\");
  if (pos == std::string::npos) return ".";
  return path.substr(0, pos);
}

static void eval_source(const std::string& src,
                        const std::vector<std::string>& args,
                        Interpreter& inter,
                        bool print_ast = false) {
  Lexer lexer;
  auto tokens = lexer.tokenize(src);

  if (g_debug_lexer) {
    std::cout << "==== Lexer Tokens ====\n";
    for (const auto& t : tokens) {
      std::cout << "  [" << tokenTypeToString(t.type) << "] "
                << "'" << t.lexeme << "'"
                << "  (line " << t.line << ", col " << t.col << ")\n";
    }
    std::cout << "======================\n";
  }

  Parser parser;
  parser.tokens = tokens;
  parser.ids = &inter.id_lookup;

  auto ast = parser.parse();
  if (print_ast) {
    printAST(ast.get());
  }

  inter.run(ast.get(), args);
}

static bool likely_incomplete(const std::string& code) {
  int bal = 0;
  for (char c : code) {
    if (c == '{') ++bal;
    else if (c == '}') --bal;
  }
  if (bal > 0) return true;
  if (!code.empty() && code.back() == '\\') return true; 
  return false;
}

// --------------------------------------------------------
// REPL
// --------------------------------------------------------
static int repl() {
  std::cout << "Rue REPL (Ctrl+D to exit)\n";
  std::cout << "Tips: end a line with '\\' or leave unmatched '{' to continue on the next line.\n";

  Interpreter inter; 
  std::vector<std::string> args; 

  std::string line;
  std::string buffer;
  bool continuation = false;

  while (true) {
    std::cout << (continuation ? "...> " : "rue> ") << std::flush;
    if (!std::getline(std::cin, line)) {
      std::cout << "\n";
      break; 
    }

    if (!line.empty() && line.back() == '\\') {
      buffer += line.substr(0, line.size() - 1) + "\n";
      continuation = true;
      continue;
    }

    buffer += line + "\n";
    continuation = likely_incomplete(buffer);

    if (continuation) {
      continue; 
    }

    // blank line -> just reprompt
    bool only_ws = true;
    for (char c : buffer) {
      if (!isspace(static_cast<unsigned char>(c))) {
        only_ws = false;
        break;
      }
    }
    if (only_ws) {
      buffer.clear();
      continue;
    }

    try {
      eval_source(buffer, args, inter, /*print_ast=*/false);
    } catch (const std::exception& e) {
      std::cerr << "\033[31mruntime error:\033[0m " << e.what() << "\n";
    } catch (...) {
      std::cerr << "\033[31mruntime error:\033[0m unknown exception\n";
    }
    buffer.clear();
  }

  return 0;
}

int main(int argc, char* argv[]) {
  // Modes:
  //   1) No args                        -> REPL
  //   2) [-i]                           -> REPL
  //   3) [--debug-lexer] -i             -> REPL with lexer debug
  //   4) [--debug-lexer] file.rue ...   -> run file, optional lexer debug

  int argi = 1;

  // optional --debug-lexer flag
  if (argi < argc && std::string(argv[argi]) == "--debug-lexer") {
    g_debug_lexer = true;
    ++argi;
  }

  if (argi >= argc || std::string(argv[argi]) == "-i") {
    return repl();
  }

  std::string fname = argv[argi];
  if (fname.size() < 4 || fname.substr(fname.size() - 4) != ".rue") {
    std::cerr << "file extension must end with '.rue'\n";
    return 1;
  }

  std::ifstream file(fname);
  if (!file.is_open()) {
    std::cerr << "\033[31mError:\033[0m Could not open file '" << fname << "'\n";
    return 1;
  }

  std::ostringstream buf;
  buf << file.rdbuf();
  std::string src = buf.str();

  std::vector<std::string> args;
  // Forward any extra CLI args to the interpreted program 
  for (int i = argi + 1; i < argc; ++i) {
    args.push_back(argv[i]);
  }

  Interpreter inter;
  inter.module_dir = dirname(fname);
  try {
    eval_source(src, args, inter, /*print_ast=*/true);
  } catch (const std::exception& e) {
    std::cerr << "\033[31mruntime error:\033[0m " << e.what() << "\n";
    return 1;
  }

  return 0;
}
