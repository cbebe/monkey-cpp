all:
	g++ -c main.cpp

compile_commands.json:
	bear -- $(MAKE) all
