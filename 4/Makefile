.PHONY: all debug release

all: debug

debug:
	clang++ -g -Wall -Wextra -std=c++17 -O0 -lstdc++ starwars_example.cc -o starwars_example
release:
	clang++ -Wall -Wextra -std=c++17 -O2 -lstdc++ starwars_example.cc -o starwars_example
