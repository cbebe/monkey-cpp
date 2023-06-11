MAINS := lexer ast parser evaluator
OBJ := \
	obj/token.o \
	obj/object.o \
	obj/environment.o \
	obj/builtins.o \
	$(MAINS:%=obj/%.o) \

TEST_SUITES := $(MAINS:%=test_%)
TEST_CMDS := $(MAINS:%=test-%)
TEST_SUITE_OBJS := $(TEST_SUITES:%=test_obj/%.o)

CPPFLAGS := -std=c++20 -Wall -Wextra -Wpedantic -O3

# {{{ Commands
help:
	@sed -n "/^# {{{ Commands/,/^# END Commands }}}/p" Makefile | grep -E "^\w+$:"

fmt:
	clang-format -i **/*.cpp **/*.hpp

lint:
	clang-tidy **/*.cpp **/*.hpp

test: $(TEST_CMDS)

test-lexer: test_lexer
	@echo "==> LEXER TEST"
	@./$<

test-ast: test_ast
	@echo "==> AST TEST"
	@./$<

test-parser: test_parser
	@echo "==> PARSER TEST"
	@./$<

test-evaluator: test_evaluator
	@echo "==> EVALUATOR TEST"
	@./$<

clean:
	rm -rf obj test_obj $(TEST_SUITES) monke_repl


clean-lsp:
	rm -f compile_commands.json

run: monke_repl
	@./$<

all: monke_repl

leak: monke_repl
	@valgrind ./$<

test-leak: test_evaluator
	@echo "==> TESTING MEMORY LEAKS"
	@valgrind ./test_evaluator
.PHONY: fmt lint $(TEST_CMDS) test-leak clean clean-lsp run leak
# END Commands }}}

# {{{ Executables
test_%: $(OBJ) test_obj/test_%.o
	g++ -o $@ $(CPPFLAGS) $^

monke_repl: $(OBJ) obj/repl.o
	g++ -o $@ $(CPPFLAGS) $^
# END Executables }}}

# {{{ Objects
obj/%.o: src/token/%.cpp src/token/%.hpp | obj
	g++ -c -o $@ $(CPPFLAGS) $<

obj/%.o: src/ast/%.cpp src/ast/%.hpp | obj
	g++ -c -o $@ $(CPPFLAGS) $<

obj/%.o: src/lexer/%.cpp src/lexer/%.hpp | obj
	g++ -c -o $@ $(CPPFLAGS) $<

obj/%.o: src/parser/%.cpp src/parser/%.hpp | obj
	g++ -c -o $@ $(CPPFLAGS) $<

obj/%.o: src/object/%.cpp src/object/%.hpp | obj
	g++ -c -o $@ $(CPPFLAGS) $<

obj/%.o: src/evaluator/%.cpp src/evaluator/%.hpp | obj
	g++ -c -o $@ $(CPPFLAGS) $<

obj/%.o: src/%.cpp | obj
	g++ -c -o $@ $(CPPFLAGS) $<

test_obj/%.o: src/%.cpp | test_obj
	g++ -c -o $@ $(CPPFLAGS) $<

obj:
	mkdir -p $@

test_obj:
	mkdir -p $@
# }}}

# did you `sudo apt install libstdc++-12-dev` ??
compile_commands.json:
	bear -- $(MAKE) all

.SECONDARY:
# vim:foldmethod=marker
