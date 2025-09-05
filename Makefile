CXX = g++
CXXFLAGS = -Wall -Wextra -Wpedantic

# Sources
AST_SRC = ./src/frontend/*.cpp ./src/midend_ast/*.cpp
IR_SRC  = ./src/frontend/*.cpp ./src/midend_ir/*.cpp

# Output binaries
AST_BIN = rue_runtime_ast
IR_BIN  = rue_falt_ir

# Default target
all: help

# Build AST binary
ast: $(AST_SRC)
	$(CXX) $(CXXFLAGS) -o $(AST_BIN) $(AST_SRC)
	@echo "AST program built: $(AST_BIN)"

# Build IR binary
ir: $(IR_SRC)
	$(CXX) $(CXXFLAGS) -o $(IR_BIN) $(IR_SRC)
	@echo "IR program built: $(IR_BIN)"

# Clean
clean:
	rm -f $(AST_BIN) $(IR_BIN)
	@echo "Cleaned binaries"

# Help
help:
	@echo "Usage:"
	@echo "  make ast   - Build the AST program"
	@echo "  make ir    - Build the IR program"
	@echo "  make clean - Remove binaries"

