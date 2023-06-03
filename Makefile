OBJ := \
	obj/token.o \
	obj/lexer.o \
	obj/ast.o \
	obj/parser.o \

TEST_OBJ := \
	test_obj/parser_test.o \
	test_obj/lexer_test.o \
	test_obj/ast_test.o \

CPPFLAGS := -std=c++20 -Wall -Wextra -pedantic -O3

# {{{ Commands
help:
	@sed -n "/^# {{{ Commands/,/^# END Commands }}}/p" Makefile | grep -E "^\w+$:"

fmt:
	clang-format -i **/*.cpp **/*.hpp

lint:
	clang-tidy **/*.cpp **/*.hpp

test: test-lexer test-ast test-parser

test-lexer: test_lexer
	@echo "==> LEXER TEST"
	@./$<

test-ast: test_ast
	@echo "==> AST TEST"
	@./$<

test-parser: test_parser
	@echo "==> PARSER TEST"
	@./$<

clean:
	rm -rf obj test_obj test_ast test_lexer test_parer monke_repl


clean-lsp:
	rm -f compile_commands.json

run: monke_repl
	@./$<

leak: monke_repl
	@valgrind ./$<

.PHONY: fmt lint test-lexer test-ast test-parser clean clean-lsp run leak
# END Commands }}}

# {{{ Executables
test_lexer: $(TEST_OBJ) $(OBJ) test_obj/test_lexer.o
	g++ -o $@ $(CPPFLAGS) $^

test_ast: $(TEST_OBJ) $(OBJ) test_obj/test_ast.o
	g++ -o $@ $(CPPFLAGS) $^

test_parser: $(TEST_OBJ) $(OBJ) test_obj/test_parser.o
	g++ -o $@ $(CPPFLAGS) $^

monke_repl: $(OBJ) obj/repl.o
	g++ -o $@ $(CPPFLAGS) $^
# END Executables }}}

# {{{ Objects
obj/%.o: src/lexer/%.cpp src/lexer/%.hpp | obj
	g++ -c -o $@ $(CPPFLAGS) $<

obj/%.o: src/parser/%.cpp src/parser/%.hpp | obj
	g++ -c -o $@ $(CPPFLAGS) $<

obj/%.o: src/ast/%.cpp src/ast/%.hpp | obj
	g++ -c -o $@ $(CPPFLAGS) $<

test_obj/%.o: src/lexer/%.cpp src/lexer/%.hpp | test_obj
	g++ -c -o $@ $(CPPFLAGS) $<

test_obj/%.o: src/ast/%.cpp src/ast/%.hpp | test_obj
	g++ -c -o $@ $(CPPFLAGS) $<

test_obj/%.o: src/parser/%.cpp src/parser/%.hpp | test_obj
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

# vim:foldmethod=marker
