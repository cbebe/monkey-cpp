OBJ := token.o lexer.o
CPPFLAGS := -std=c++20
all: monke
	./$<

monke: $(OBJ) main.o
	g++ -o $@ $(CPPFLAGS) $^

%.o: %.cpp
	g++ -c -o $@ $(CPPFLAGS) $<

test: monke_test
	./$<

monke_test: $(OBJ) test.o
	g++ -o $@ $(CPPFLAGS) $^

compile_commands.json:
	bear -- $(MAKE) all

clean:
	rm -f *.o compile_commands.json monke monke_test

.PHONY: clean test
