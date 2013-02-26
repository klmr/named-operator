# What is wheels?

Wheels is just a library where I collect stuff I think I can reuse.

# Compilers

Many compilers currently support only a small subset of the interesting C++
features. As of this writing, the only compilers with enough support to compile
wheels are GCC 4.7 (clang 3.1 has a couple of issues I'll investigate soon).
There are no plans to support any older compilers, or compilers with less
support for C++ features.

# Dependencies

Boost 1.49 is required. It may work with older versions, but I don't plan on
supporting them.

# Usage

Wheels is header-only: you can drop the header files somewhere and `#include`
them.

# Documentation

Documentation is forthcoming. For now the best documentation is the tests and
the comments in the code.

# Thanks

Special thanks go to the person that goes by "Luc Danton" in the
[Lounge<C++>][lounge] for several ideas.

 [lounge]: http://chat.stackoverflow.com/rooms/10

