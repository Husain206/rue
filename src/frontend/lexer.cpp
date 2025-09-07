#include "lexer.h"
#include <cctype>
#include <unordered_map>


bool Lexer::isAtEnd() { return offset >= src.size(); }

char Lexer::peek() { return (isAtEnd()) ? '\0' : src[offset]; }

char Lexer::peekNext() { return (offset + 1 >= src.size()) ? '\0' : src[offset + 1]; }

char Lexer::peekPre() { return (offset == 0 || isAtEnd()) ? '\0' : src[offset - 1]; }

char Lexer::advance() { return (isAtEnd()) ? '\0' : src[offset++]; }

char Lexer::next() {
  if(isAtEnd()) return '\0';
  char c = advance();
    if (c == '\n') {
    col = 0;
    line++;
    } else col++;

    return c;
}

bool Lexer::match(char c) {
  if (peek() == c) {
    next();
    return true;
  }
  return false;
}

void Lexer::error(str msg) {
  std::cerr << "main.ru:" << line << ":" << col << ": Error: " << msg
            << std::endl;

    exit(0);
}

void Lexer::isWhiteSpace() {
  while (!isAtEnd()) {
    if ((peek() == '/' && peekNext() == '/') || peek() == '#') {
      while (peek() != '\n') {
        next();
      }
    } else if (std::isspace(peek())) {
      next();
    } else
      break;
  }
}

 Token Lexer::symbolToken(){
  char c = next();
  if (isAtEnd()) return {EoF, "", line, col};
  
  usize a = col;
  usize l = line;
  switch (c) {
     case '+': if(match('+')) return {INC, "++", line, a}; else return {PLUS, "+", line, a};
     case '-': if(match('-')) return {DEC, "--", line, a}; else return {MINUS, "-", line, a};
     case '*': if(match('*')) return {POW, "**", line, a}; else return {STAR, "*", line, a};
     case '/': return {SLASH, "/", line, a};
     case ',': return {COMMA, ",", line, a};
     case '.': return {DOT, ".", line, a};
     case ';': return {SEMIC, ";", line, a};
     case '{': /*bracketDeep.push_back(offset);*/ return {LCB, "{", l, a};
     case '}': /*(bracketDeep.empty()) ? error("unmatched '}' - missing '{'") : bracketDeep.pop_back();*/ return {RCB, "}", line, a};
     case '(': return {LPRN, "(", line, a};
     case ')': return {RPRN, ")", line, a};
     case '[': return {LB, "[", line, a};
     case ']': return {RB, "]", line, a};
     case '=': if(match('=')) return {EQEQ, "==", line, a}; else return {};
     case '>': if(match('=')) return {GQ, ">=", line, a}; else if(match('>')) return {BITWISE_RIGHT_SHIFT, ">>", line, a};  else return {GRT, ">", line, a};
     case '<': if(match('=')) return {LQ, "<=", line, a}; else if(match('<')) return {BITWISE_LEFT_SHIFT, "<<", line, a};  else return {LST, "<", line, a};
     case '|': if(match('|')) return {OR, "||", line, a};   else return {BITWISE_OR, "|", line, a};
     case '^': return {BITWISE_XOR, "^", line, a};
     case '~': return {BITWISE_NOT, "~", line, a};
     case '&': if(match('&')) return {AND, "&&", line, a};  else return {ADDR, "&", line, a};
     case '!': return {NOT, "!", line, a};
     case '%': return {MOD, "%", line, a};
     case ':': if(match('=')) return {EQ, ":=", line, a}; else return {COLON, ":", line, a};
     case '?': return {QUES, "?", line, a};

     case '"': return toString();
     case '\'': return toAscii();
  }
  return {INVALID ,"", line, a};
}

Token Lexer::toAscii() {
    if (isAtEnd()) error("Unterminated character literal.\n");

    str ch;
    char c = next();

    if (c == '\\') { 
        char e = next();
        switch (e) {
            case 'n': ch = "\n"; break;
            case 't': ch = "\t"; break;
            case 'r': ch = "\r"; break;
            case '0': ch = "\0"; break;
            case '\'': ch = "\'"; break;
            case '\\': ch = "\\"; break;
            default:
                error(std::string("unknown escape sequence \\") + e + "\n");
        }
    } else {
        ch = c;
    }

    if (peek() != '\'') {
        error("Character literal too long or unterminated.\n");
    }
    next(); 

    return {ASCII_CH, ch, line, col};
}


Token Lexer::toString() {
  str string;
  while (peek() != '"' && !isAtEnd()) {
if (peek() == '\\') {
    next(); 
    char esc = next(); 
    switch (esc) {
        case 'n':  string += '\n'; break;
        case '0':  string += '\0'; break;
        case 't':  string += '\t'; break;
        case 'r':  string += '\r'; break;
        case '\\': string += '\\'; break;
        case 'b':  string += '\b'; break;
        case 'f':  string += '\f'; break;
        case '"':  string += '"';  break;
        default:
            error(std::string("unknown escape sequence \\") + esc);
    }
} else 
    string += next();
}
    

  if (isAtEnd()) error("Unterminated string.\n");
  next();
  return {STRLIT, string, line, col};
}

Token Lexer::setKeyword(str iden) {
  static const std::unordered_map<str, TokenType> keywords = {
    {"set", SET},
    {"print", PRINT},
    {"input", INPUT},
    {"ala", ALA},
    {"if", IF},
    {"else", ELSE},
    {"for", FOR},
    {"fn", FN},
    {"return", RETURN},
    {"null", NALL},
    {"continue", CONTINUE},
    {"break", BREAK}
  };

  auto it = keywords.find(iden);
  if (it != keywords.end()) {
    return {it->second, iden, line, col};
  }
  return {IDENT, iden, line, col};
}

Token Lexer::ID() {
  str iden;
  if(std::isdigit(peekPre())){
    error("cannot start an identifier with a number\n");
    }
    if (std::isalpha(peek()) || peek() == '_') {
    while (isalnum(peek()) || peek() == '_') {
      iden += next();
    }  
  }
  
  return setKeyword(iden);
}

Token Lexer::Number() {
  str num;
  while (std::isdigit(peek()) || peek() == '.') {
    num += next();
  }
  return {NUMLIT, num, line, col};
}

std::vector<Token> Lexer::tokenize(const str &source) {
  src = source;
  std::vector<Token> tokens;

  while (!isAtEnd()) {
    if(isAtEnd()){
      tokens.push_back({EoF, "", line, col});
      break;
    }
    
    
    isWhiteSpace();
    if (std::isalpha(peek()) || peek() == '_') {
      tokens.push_back(ID());
    } else if (isdigit(peek()) || peek() == '.') {
      tokens.push_back(Number());
    } else {
       Token t = symbolToken();
      if (t.type == INVALID && t.lexeme.empty()) continue;
      tokens.push_back(t);
    }
  }
  /*if(!bracketDeep.empty()){
        error("unmatched '{' - missing '}'");
      }*/

  return tokens;
}
