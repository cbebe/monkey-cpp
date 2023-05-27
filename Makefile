SRC := token.cpp lexer.cpp
all: monke
	./$<

monke: main.cpp $(SRC)
	g++ -o $@ -std=c++20 $<

test: monke_test
	./$<

monke_test: test.cpp $(SRC)
	g++ -o $@ -std=c++20 $<

compile_commands.json:
	bear -- $(MAKE) all

clean:
	rm -f main.o compile_commands.json

.PHONY: clean test
