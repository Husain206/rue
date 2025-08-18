
#include "parser.h"
#include "ast.h"
#include "lexer.h"
#include <iostream>
#include <memory>

Token Parser::peek() {
  return (!isAtEnd()) ? tokens[current] : Token{EoF, "", 0, 0};
}

// Token Parser::peekNext() { return (!isAtEnd()) ? tokens[current + 1] :
// Token{EoF, "", 0, 0}; }

// Token Parser::peekPre() { return (!isAtEnd()) ? tokens[current - 1] :
// Token{EoF, "", 0, 0}; }

Token Parser::next() {
  if (!isAtEnd())
    current++;
  return tokens[current - 1];
}

bool Parser::isAtEnd() {
  return current >= tokens.size() || tokens[current].type == EoF;
}

bool Parser::check(TokenType type) { return peek().type == type; }

bool Parser::match(TokenType type) {
  if (check(type)) {
    next();
    return true;
  }
  return false;
}

void Parser::consume(const TokenType &t, str msg) {
  if (!check(t)) {
    std::cerr << "main.ru:" << peek().line << ":" << peek().col
              << ": Error: " << msg;
    exit(0);
  }
  next();
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
  case LST:
  case GRT:
  case LQ:
  case GQ:
  case EQEQ:
  case NOTEQ:
  case OR:
  case AND:
  case MOD:
    return COMP;
  case NOT:
  case ADDR:
  case DEREF:
  case INC:
  case DEC:
    return PREFIX;
    // case INC:
    // case DEC:
    //   return POSTFIX;
  default:
    return NONE;
  }
}

unique_ptr<Node> Parser::parse() {
  auto block = make_unique<Node>(n_expr_stmt);
  while (!isAtEnd()) {
    auto s = parse_stmt();
    if (s)
      block->children.push_back(std::move(s));
    else
      break;
  }
  return block;
}

unique_ptr<Node> Parser::parse_stmt() {
  if (check(SET)) {
    return parse_set();
  } else if (check(PRINT)) {
    return parse_print();
  } else if (check(ALA)) {
    return parse_ala();
  } else if (check(IF)) {
    return parse_if();
  } else if (check(FOR)) {
    return parse_for_loop();
    // } else if(check(RETURN)){
    //   parse_return();
    // } else if(check(FN)){
    //   parse_fn();
  }

  auto expr = parseExpr();
  if (expr) {
    consume(SEMIC, "Expected ';' after expression");
    auto node = make_unique<Node>(n_expr_stmt);
    node->children.push_back(std::move(expr));
    return node;
  }

  return nullptr;
}

unique_ptr<Node> Parser::parse_set() {
  next();
  Token name = next();
  if (name.type != IDENT) {
    std::cerr << "Expected identifier after 'set' at " << peek().line << ":"
              << peek().col << "\n";
    return nullptr;
  }

  consume(EQ, "Expected '=' after identifier in set\n");
  auto value = parseExpr();
  consume(SEMIC, "Expected ';' at the end of set\n");

  auto node = make_unique<Node>(n_set);
  auto ident = make_unique<Node>(n_id);
  ident->lexeme = name.lexeme;
  node->children.push_back(std::move(ident));
  node->children.push_back(std::move(value));

  return node;
}

unique_ptr<Node> Parser::parse_print() {
  next();

  auto value = parseExpr();
  consume(SEMIC, "Expected ';' at the end of print\n");

  auto node = make_unique<Node>(n_print);
  node->children.push_back(std::move(value));

  return node;
}

unique_ptr<Node> Parser::parse_ala() {
  next();
  consume(LPRN, "expected '(' after ala\n");
  auto cond = parseExpr();
  consume(RPRN, "expected ')' after condition in ala\n");
  consume(LCB, "expected '{' after ')' in ala\n");

  auto node = make_unique<Node>(n_block);
  while (!check(RCB) && !isAtEnd()) {
    auto s = parse_stmt();
    if (s)
      node->children.push_back(std::move(s));
    else
      break;
  }
  consume(RCB, "expected '}' at the end of ala\n");

  auto wrapper = make_unique<Node>(n_ala);
  wrapper->children.push_back(std::move(cond));
  wrapper->children.push_back(std::move(node));
  return wrapper;
}

unique_ptr<Node> Parser::parse_if() {
  next();
  consume(LPRN, "expected '(' after if\n");
  auto cond = parseExpr();
  consume(RPRN, "expected ')' after condition in if\n");
  consume(LCB, "expected '{' after ')' in if\n");

  auto body = make_unique<Node>(n_block);
  while (!check(RCB) && !isAtEnd()) {
    auto s = parse_stmt();
    if (s)
      body->children.push_back(std::move(s));
    else
      break;
  }
  consume(RCB, "expected '}' at the end of if\n");

  auto node = make_unique<Node>(n_if);
  node->children.push_back(std::move(cond));
  node->children.push_back(std::move(body));

  if (match(ELSE)) {
    consume(LCB, "expected '{' after ')' in if\n");
    auto elseBody = make_unique<Node>(n_else);

    while (!check(RCB) && !isAtEnd()) {
      auto s = parse_stmt();
      if (s)
        elseBody->children.push_back(std::move(s));
      else
        break;
    }

    consume(RCB, "expected '}' at the end of if\n");
    node->children.push_back(std::move(elseBody));
  }
  return node;
}

// for (set i = 0; i < 10; i++)

unique_ptr<Node> Parser::parse_for_loop(){
    next();
    consume(LPRN,"expected '(' after loop\n");

    unique_ptr<Node> init = nullptr;
    if(check(SET)) init = parse_set();

    else {
      if(check(SEMIC)) next();
      else std::cerr << "Expected 'set' or ';' in for-init\n";
    }

    auto cond = parseExpr();
    consume(SEMIC,"Expected ';' after condition in for loop\n");

    auto inc = parseExpr();
    consume(RPRN,"expected ')' after condition in for loop\n" );

    consume(LCB,"expected '{' after ')' in for loop\n" );

    auto body = make_unique<Node>(n_block);
    while(!check(RCB) && !isAtEnd()){
      auto s = parse_stmt();
      if(s) body->children.push_back(std::move(s));
      else break;
    }
    consume(RCB,"expected '}' at the end of for loop\n" );

    auto node = make_unique<Node>(n_for_loop);
    node->children.push_back(std::move(init)); // could be nullptr
    node->children.push_back(std::move(cond));
    node->children.push_back(std::move(inc));
    node->children.push_back(std::move(body));

    return node;
}

unique_ptr<Node> Parser::parseExpr(int rbp) {
  Token t = next();
  unique_ptr<Node> left = nud(t);

  while (!isAtEnd() && rbp < get_prec(peek().type)) {
    Token op = next();
    left = led(op, std::move(left));
  }

  return left;
}

unique_ptr<Node> Parser::nud(const Token &t) {
  switch (t.type) {
  case ASCII_CH:
  case STRLIT: {
    auto node = make_unique<Node>(n_str);
    node->lexeme = t.lexeme;
    return node;
  }
  case IDENT: {
    auto node = make_unique<Node>(n_id);
    node->lexeme = t.lexeme;
    return node;
  }
  case NUMLIT: {
    auto node = make_unique<Node>(n_num);
    node->lexeme = t.lexeme;
    return node;
  }
  case LPRN: {
    unique_ptr<Node> inner = parseExpr();
    if (!match(RPRN))
      std::cerr << "expected ')' at: " << t.line << ":" << t.col
                << std::endl; // maybe consume
    return inner;
  }

  case MINUS:
  case PLUS:
  case NOT:
  case INC:
  case DEC:
  case ADDR:
  case DEREF: {
    auto node = make_unique<Node>(n_unary);
    node->op = t.type;
    node->lexeme = t.lexeme;
    node->children.push_back(parseExpr(lbp(t)));
    return node;
  }
  default:
    std::cerr << "unexpected token: '" << t.lexeme << "' :" << t.line << ":"
              << t.col << std::endl;
    return nullptr;
  }
}

unique_ptr<Node> Parser::led(const Token &t, unique_ptr<Node> left) {
 switch(t.type){ 
  case PLUS:
  case MINUS:
  case STAR:
  case SLASH:
  case MOD:
  case EQEQ:
  case NOTEQ:
  case AND:
  case OR:
  case LST:
  case GRT:
  case LQ:
  case GQ:{
      auto n = make_unique<Node>(n_binary);
      n->op = t.type;
      n->lexeme = t.lexeme;
      n->children.push_back(std::move(left));
      n->children.push_back(parseExpr(lbp(t)));
      return n;
      }
  case EQ: {
         auto n = make_unique<Node>(n_assign);
         n->op = t.type;
         n->lexeme = t.lexeme;
         n->children.push_back(std::move(left));
         n->children.push_back(parseExpr(lbp(t) - 1));
         return n;
       }
   default:
     std::cerr << "Unhandled infix operator: " << t.lexeme << "\n";
     return left;
    }
}

int Parser::lbp(const Token &t) { return get_prec(t.type); }
