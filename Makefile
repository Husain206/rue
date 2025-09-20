CXX = g++
CXXFLAGS = -Wall -Wextra -Wpedantic

AST_SRC = $(wildcard src/frontend/*.cpp src/midend_ast/*.cpp)
IR_SRC  = $(wildcard src/frontend/*.cpp src/midend_ir/*.cpp)

AST_BIN = rue_runtime_ast
IR_BIN  = rue_flat_ir

all: help

ast: $(AST_BIN)

$(AST_BIN): $(AST_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

ir: $(IR_BIN)

$(IR_BIN): $(IR_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(AST_BIN) $(IR_BIN)
	@echo "Cleaned binaries"

help:
	@echo "Usage:"
	@echo "  make ast   - Build the AST program"
	@echo "  make ir    - Build the IR program"
	@echo "  make clean - Remove binaries"
