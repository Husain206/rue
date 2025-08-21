# Rue

my new little langauge, named rue :D
Rue is a small, C/JS-like programming language implemented in C++. 
Rue is a learning project so far, it doesnt follow the best practies or whatever, i tried to make the code structure as simple and understandable as possiable and reflecting the programming style i like

it is desgined in C++ with a C-like Style while benefeting from the STL c++ provides
i tried not to use OOP as much as possiable becasue OOP sucks, i think i only used once or twice? idr

**fun fact:** rue means "regret" but it also refers to a medical herb which :D

## Table of Contents

- [Usage](#usage)
- [Project Structure](#project-structure)
- [Language Syntax](#language-syntax)

## Usage

1. Clone the repository:

*note:* aaa ill put the Makfile in the main dir later too lazy for now :D
```bash
git clone git@github.com:Husain206/rue.git
cd rue/src
make
./rue tests/[file.ru]
```
## project structure
```
rue
|
|---> src/     -> rue.cpp // main entry 
|       |_->   -> lexer.h lexer.cpp           // the tokenizer
|       |_->   -> parser.h parser.cpp ast.h   // syntatic analysis
|       |_->   -> ast.cpp                     // output the AST tree
|       |_>    -> runtime.h runtime.cpp       // evaluator and symbol-table(semantic analysis) // im gonna seperate them soon(already did)
|       |_>    -> Makefile                    // build file
|       |_>    tests/                         // holds tests files for the langauge     
|
|---> .gitignore
|---> README.md
```

## Language Syntax

**declatring a variable and printing:**
```rue.ru
set num1 := 23;
set num2 := 3;
set res := num1 * num2;
print res; // 69
res = 6 + "9"; // concatination
print res; // 69 
```
i got the idea of doing := recently so i hardcoded it so it's only working when you try to declare variable, soon ill fix it :D
print print with a new line :D, i mean yeah i should have named it println but whatever eventually ill swtich to the standard lib so whatever :D

### control flow:

**true & false** are builtin variables they evaluate to 1 and 0 respectivly
ala loop is just while loop stands for *as long as* :D

so far only prefix inc or dec works (++i || --i), postfix ones are comming soon

comparison and bitwise operators are all working

```rue.ru
ala(true) {
   if(false){
     print "true";
   } else {
       print "false";
     }
}
```
```rue.ru
for(set i := 0; i < 10; ++i){
  if(i & 2 == 0) {  // ( !(i & 1) ) also works 
     print "even";
  } else {
      print "odd";
    }
}
```
also parenthesis around ala and if are not neccasry you can just do:
```rue.ru
if 1 < 2 {
   print "true";
}
```
