# IDontCareAboutMakefiles(TM).
CXX=g++

DEBUG_FLAGS=-Wall -Wextra -Wshadow -std=c++17 -g -O0 -DDEBUG
RELEASE_FLAGS=-Wall -Wextra -std=c++17 -O2

# 'debug' is a defaul target. To build with debug 'make release' must be called.
CFLAGS=$(DEBUG_FLAGS)

EXECUTABLE_NAME=main

# Find all target .o files based on .c files.
OBJECTS=$(shell for i in *.cc; do echo "$${i%.cc}.o" ; done)

.PHONY: all debug clean post_hook

all: $(EXECUTABLE_NAME) post_hook

release: CFLAGS=$(RELEASE_FLAGS)
release: all

$(EXECUTABLE_NAME): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXECUTABLE_NAME)

.cc.o:
	$(CXX) $(CFLAGS) -c $< -o $@

clean: post_hook
	@-rm -f *.o
	@-rm -f $(EXECUTABLE_NAME)

format:
	for f in *.c *.h *.cpp *.hpp *.cc; do 	\
		if [ -f $$f ] ; then 		\
			clang-format -i $$f;	\
		fi				\
	done

# Make a .dep file for every .c file using $(CXX) -MM. This will auto-generate
# file dependencies.
%.dep : %.cc
	@$(CXX) -MM $(CFLAGS) $< > $@
include $(OBJECTS:.o=.dep)

# No trash in the working directory; remove all .dep files after compilation.
# The downside of generationg .dep fiels is then [post_hook] must be called
# after any command now, and may confuse other users.
post_hook:
	@-rm *.dep
