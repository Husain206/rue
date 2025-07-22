#include "parser.h"
#include "lexer.h"

Token Parser::peek() { return (!isAtEnd()) ? tokens[current] : Token{EoF, "", peek().line, peek().col}; }

Token Parser::peekNext() { return (!isAtEnd()) ? tokens[current + 1] : Token{EoF, "", peek().line, peek().col}; }

Token Parser::peekPre() { return (!isAtEnd()) ? tokens[current - 1] : Token{EoF, "", peek().line, peek().col}; }

Token Parser::next() {
  if (!isAtEnd())
    current++;
  return tokens[current - 1];
}

bool Parser::isAtEnd() { return current >= tokens.size() || tokens[current].type == EoF; }

bool Parser::check(TokenType type) { return peek().type == type; }

bool Parser::match(TokenType type) {
  if (check(type)) {
    next();
    return true;
  }
  return false;
}


prec get_prec(TokenType type) {
  switch (type) {
  case EQ:
    return ASS;
  case PLUS:
  case MINUS:
    return SUM;
  case STAR:
  case SLASH:
    return PRODUCT;
  default:
    return NONE;
  }
}

void Parser::parse() {
  while (!isAtEnd()) {
    parse_stmt();
  }
}

void Parser::parse_stmt() {
    if (match(SET)) {
    parse_set();
  } else if (match(PRINT)) {
    parse_print();
  } else {
    std::cerr << "Unknown statement at token: " << peek().lexeme << "' (type: " << peek().type << ")\n";
    next();
    // parseExpr();
  }
}

void Parser::parse_set() {
  Token name = next();
  if (name.type != IDENT) {
    std::cerr << "Expected identifier after 'set'\n";
    return;
  }

  if (!match(EQ)) {
    std::cerr << "Expected '=' after identifier\n";
    return;
  }

  str value = parseExpr();
  if (!match(SEMIC)) {
    std::cerr << "Expected ';'\n";
    return;
  }

  std::cout << "[set] " << name.lexeme << " = " << value << "\n";
}

void Parser::parse_print() {
  str value = parseExpr();
  if (!match(SEMIC)) {
    std::cerr << "Expected ';'\n";
    return;
  }

  std::cout << "[print] " << value << "\n";
}


str Parser::parseExpr(int rbp) {
  Token t = next();
  str left = nud(t);

  while (!isAtEnd() && rbp < lbp(peek())) {
    Token op = next();
    left = led(op, left);
  }

  return left;
}

str Parser::nud(const Token &t) {
  
  if (t.type == NUMLIT || t.type == IDENT) {
    return t.lexeme;
  }

  if (t.type == LPRN) {
    str val = parseExpr();
    if (!match(RPRN)) {
      std::cerr << "Expected ')'\n";
    }
    return "(" + val + ")";
  }

  if (t.type == MINUS) {
    str right = parseExpr(PREFIX);
    return "(-" + right + ")";
  }

  std::cerr << "Unexpected token in nud: " << t.lexeme << "\n";
  return "<?>";
}

str Parser::led(const Token &t, str left) {
  str right = parseExpr(get_prec(t.type));
  return "(" + left + " " + t.lexeme + " " + right + ")";
}

int Parser::lbp(const Token &t) { return get_prec(t.type); }



