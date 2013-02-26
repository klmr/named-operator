WHEELS=deps/wheels/include
INC=include

CXX=g++
CXXFLAGS=-std=c++11 -pedantic -Werror -Wall -Wextra \
		 -I$(INC) -I$(WHEELS)

example: $(INC)/base/named_operator.hpp $(INC)/util/io_helpers.hpp
