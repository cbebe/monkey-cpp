OBJ := obj/token.o obj/lexer.o obj/ast.o obj/parser.o
CPPFLAGS := -std=c++20

run: monke_repl
	@./$<

all: monke_repl

monke_repl: $(OBJ) repl.o
	g++ -o $@ $(CPPFLAGS) $^

%.o: %.cpp
	g++ -c -o $@ $(CPPFLAGS) $<

obj/%.o: src/%.cpp src/%.hpp | obj
	g++ -c -o $@ $(CPPFLAGS) $<

obj:
	mkdir -p $@

test: monke_test
	@./$<

monke_test: $(OBJ) test.o
	g++ -o $@ $(CPPFLAGS) $^

compile_commands.json:
	bear -- $(MAKE) all

clean-lsp:
	rm -f compile_commands.json

clean:
	rm -rf *.o obj monke_repl monke_test

.PHONY: clean clean-lsp test lsp run all
