OBJ := \
	obj/token.o \
	obj/lexer.o \
	obj/ast.o \
	obj/parser.o \
	obj/expressions.o \
	obj/statements.o \

TEST_OBJ := \
	test_obj/test_next_token.o \
	test_obj/test_statements.o \
	test_obj/test_expressions.o \

CPPFLAGS := -std=c++20 -Wall -Wextra -pedantic -O3

parser: monke_repl
	@./$<

lexer: monke_repl
	@./$< lex

all: monke_repl

monke_repl: $(OBJ) repl.o
	g++ -o $@ $(CPPFLAGS) $^

%.o: %.cpp
	g++ -c -o $@ $(CPPFLAGS) $<

obj/%.o: src/%.cpp src/%.hpp | obj
	g++ -c -o $@ $(CPPFLAGS) $<

obj/%.o: src/parser/%.cpp src/parser.hpp | obj
	g++ -c -o $@ $(CPPFLAGS) $<

test_obj/%.o: tests/%.cpp tests/tests.hpp | test_obj
	g++ -c -o $@ $(CPPFLAGS) $<

time: clean
	time $(MAKE) -j all

obj:
	mkdir -p $@

test_obj:
	mkdir -p $@

test: monke_test
	@./$<

monke_test: $(OBJ) $(TEST_OBJ) test.o
	g++ -o $@ $(CPPFLAGS) $^

# did you `sudo apt install libstdc++-12-dev` ??
compile_commands.json:
	bear -- $(MAKE) all

clean-lsp:
	rm -f compile_commands.json

clean:
	rm -rf *.o obj test_obj monke_repl monke_test

.PHONY: clean clean-lsp test lsp run all time
