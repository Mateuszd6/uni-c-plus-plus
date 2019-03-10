CC = gcc
CXX = g++

DEBUG_FLAGS = -O0 -g
RELEASE_FLAGS = -O2

FLAGS = $(RELEASE_FLAGS)

all:
# Modules:
	$(CXX) -Wall -Wextra $(FLAGS) -std=c++17 -c strset.cc -o strset.o
	$(CXX) -Wall -Wextra $(FLAGS) -std=c++17 -c strsetconst.cc -o strsetconst.o
# Usage examples:
	$(CXX) -Wall -Wextra $(FLAGS) -std=c++17 -c strset_test2a.cc -o strset_test2a.o
	$(CXX) -Wall -Wextra $(FLAGS) -std=c++17 -c strset_test2b.cc -o strset_test2b.o
	$(CC) -Wall -Wextra $(FLAGS) -std=c11 -c strset_test1.c -o strset_test1.o
	$(CXX) strset_test1.o strsetconst.o strset.o -o strset1
	$(CXX) strset_test2a.o strsetconst.o strset.o -o strset2a
	$(CXX) strset_test2b.o strsetconst.o strset.o -o strset2b
