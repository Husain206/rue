#include "parser.h"
#include "lexer.h"


Token Parser::peek() { return (!isAtEnd()) ? tokens[current] : Token{EoF, "", 0, 0}; }

// Token Parser::peekNext() { return (!isAtEnd()) ? tokens[current + 1] : Token{EoF, "", 0, 0}; }

// Token Parser::peekPre() { return (!isAtEnd()) ? tokens[current - 1] : Token{EoF, "", 0, 0}; }

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

void Parser::consume(const TokenType& t, str msg){
  if(!check(t)){
  std::cerr << "main.ru:" << peek().line << ":" << peek().col << ": Error: " << msg
            << std::endl;
    return;
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

void Parser::parse() {
  while (!isAtEnd()) {
    parse_stmt();
  }
}

void Parser::parse_stmt() {
    if (check(SET)) {
    parse_set();
  } else if (check(PRINT)) {
    parse_print();
  } else if(check(ALA)){
    parse_ala();
  } else if(check(IF)){
    parse_if();
  } else if(check(FOR)){
    parse_for_loop();
  // } else if(check(RETURN)){
  //   parse_return();
  // } else if(check(FN)){
  //   parse_fn();
  } else {
    consume(peek().type, "unknown statments at token: " + peek().lexeme);
    next();
    // parseExpr();
  }
}

void Parser::parse_set() {
  next();
  Token name = next();
  if (name.type != IDENT) {
    std::cerr << "Expected identifier after 'set' at " << peek().line << ":" << peek().col << "\n";
    return;
  }
  

    consume(EQ,"Expected '=' after identifier in set\n");
 
  str value = parseExpr();

    consume(SEMIC,"Expected ';' at the end of set\n");
}

void Parser::parse_print() {
  next();
  
  str value = parseExpr();
  consume(SEMIC, "Expected ';' at the end of print\n");
}


void Parser::parse_ala(){
  next();
     consume(LPRN,"expected '(' after ala\n" );
  str cond = parseExpr();
     consume(RPRN,"expected ')' after condition in ala\n" );
     consume(LCB,"expected '{' after ')' in ala\n" );
  while(!check(RCB) && !isAtEnd()) parse_stmt();
     consume(RCB,"expected '}' at the end of ala\n" );
}


void Parser::parse_if(){
  next();
     consume(LPRN,"expected '(' after if\n" );
     str cond = parseExpr();
     consume(RPRN,"expected ')' after condition in if\n" );
     consume(LCB,"expected '{' after ')' in if\n" );
     while(!check(RCB) && !isAtEnd()) parse_stmt();
     consume(RCB,"expected '}' at the end of if\n" );

     if(match(ELSE)){
     consume(LCB,"expected '{' after ')' in if\n" );
     while(!check(RCB) && !isAtEnd()) parse_stmt();
     consume(RCB,"expected '}' at the end of if\n" );
     }

}
// for (set i = 0; i < 10; i++)


void Parser::parse_for_loop(){
  next();
  consume(LPRN,"expected '(' after loop\n");

  if(check(SET)) parse_set();
  else consume(SET , "Expected set statement in for loop init");
  
  str cond = parseExpr();
  consume(SEMIC,"Expected ';' after condition in for loop\n");
  
  str valueInc = parseExpr();
  consume(RPRN,"expected ')' after condition in for loop\n" );
  
  consume(LCB,"expected '{' after ')' in for loop\n" );
  while(!check(RCB) && !isAtEnd()) parse_stmt();
  consume(RCB,"expected '}' at the end of for loop\n" );
}

// void Parser::parse_return(){
//   next();
//   Token id = next();
//   if(id.type != IDENT){
//      std::cerr << "Expected identifier after 'return' at " << peek().line << ":" << peek().col << "\n";
//      return;
//   }
//   consume(SEMIC,"Expected ';' after " + id.lexeme + " in return statement\n");
// }

// std::vector<Token> Parser::parse_params(){
//    std::vector<Token> params;
//    if(check(RPRN)) return params;

//    do {
//      Token param = next();
//      if (param.type != IDENT) {
//         std::cerr << "Expected identifier in parameter list at " << param.line << ":" << param.col << "\n";
//       break;
//      }
//      params.push_back(param);
//    } while(match(COMMA));

//    return params;
// }

// void Parser::parse_fn(){
//   next();
//   Token name = next();
//   if(name.type != IDENT){
//      std::cerr << "Expected identifier after 'fn' at " << peek().line << ":" << peek().col << "\n";
//      return;
//   }
//    consume(LPRN, "expected '(' after " + name.lexeme + "\n" );
//    std::vector<Token> params = parse_params();
//    consume(RPRN, "expected ')' after declaring parameters in fn " + name.lexeme + "\n");
//    consume(LCB, "expected '{' after ')' in fn " + name.lexeme + "\n");
//    while(!check(RETURN) && !isAtEnd()) parse_stmt();
//    if(!check(RETURN)){
//      std::cerr << "expetced return statment in the end of 'fn " << name.lexeme << "' at "<< peek().line << ":" << peek().col << "\n";
//      return;
//    } else {
//    parse_return();
//    }
//    consume(RCB, "expected '}' at the end of fn " + name.lexeme + "\n");
// }


str Parser::parseExpr(int rbp) {
  Token t = next();
  str left = nud(t);

  while (!isAtEnd() && rbp < get_prec(peek().type)) {
    Token op = next();
    left = led(op, left);
  }

  return left;
}

str Parser::nud(const Token &t) {
  switch (t.type) {
    case IDENT: {
        return t.lexeme;
    }
    case NUMLIT:
        return t.lexeme;
    
    case LPRN: {
         str inner = parseExpr();
         if(!match(RPRN)) std::cerr << "expected ')' at: " << t.line << ":" << t.col << std::endl;
         return "(" + inner + ")";
    }
    case MINUS:
      return "-" + parseExpr(lbp(t));

    case PLUS:
      return "+" + parseExpr(lbp(t));

    case NOT:
      return "!" + parseExpr(lbp(t));

    case INC:
      return "++" + parseExpr(lbp(t));
      
    case DEC:
      return "--" + parseExpr(lbp(t));
      
    case ADDR:
      return "&" + parseExpr(lbp(t));
      
    case DEREF:
      return "*" + parseExpr(lbp(t));
    
    default: std::cerr << "unexpected token: '" << t.lexeme << "' :" << t.line << ":" << t.col << std::endl;
    return "";
  }
}

str Parser::led(const Token &t, str left) {
  switch (t.type) {
    case PLUS:
      return "(" + left + " + " + parseExpr(lbp(t)) + ")";
    case MINUS:
      return "(" + left + " - " + parseExpr(lbp(t)) + ")";
    case STAR:
      return "(" + left + " * " + parseExpr(lbp(t)) + ")";
    case SLASH:
      return "(" + left + " / " + parseExpr(lbp(t)) + ")";
    case MOD:
      return "(" + left + " % " + parseExpr(lbp(t)) + ")";
    case EQEQ:
      return "(" + left + " == " + parseExpr(lbp(t)) + ")";
    case NOTEQ:
      return "(" + left + " != " + parseExpr(lbp(t)) + ")";
    case AND:
      return "(" + left + " && " + parseExpr(lbp(t)) + ")";
    case OR:
      return "(" + left + " || " + parseExpr(lbp(t)) + ")";
    case LST:
      return "(" + left + " < " + parseExpr(lbp(t)) + ")";
    case GRT:
      return "(" + left + " > " + parseExpr(lbp(t)) + ")";
    case LQ:
      return "(" + left + " <= " + parseExpr(lbp(t)) + ")";
    case GQ:
      return "(" + left + " >= " + parseExpr(lbp(t)) + ")";
    case EQ:
      return "(" + left + " = " + parseExpr(lbp(t) - 1); // right-ass
            
    default:
      std::cerr << "Unhandled infix operator: " << t.lexeme << "\n";
      return left;
  }
}



int Parser::lbp(const Token &t) { return get_prec(t.type); }
