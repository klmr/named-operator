INC=include

all: example strings

CXXFLAGS+=-std=c++11 -pedantic -Werror -Wall -Wextra -I$(INC)

example: example.cpp $(INC)/base/named_operator.hpp $(INC)/util/io_helpers.hpp
	$(CXX) $(CXXFLAGS) -o $@ $<

strings: strings.cpp $(INC)/base/named_operator.hpp $(INC)/util/io_helpers.hpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	$(RM) example strings
