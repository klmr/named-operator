WHEELS=deps/wheels/include
INC=include

all: example strings

# Wheels currently doesn’t compile on clang++ yet.
CXX=g++
CXXFLAGS=-std=c++11 -pedantic -Werror -Wall -Wextra -I$(INC) -isystem$(WHEELS)

example: $(INC)/base/named_operator.hpp $(INC)/util/io_helpers.hpp

strings: $(INC)/base/named_operator.hpp $(INC)/util/io_helpers.hpp

clean:
	$(RM) example strings
