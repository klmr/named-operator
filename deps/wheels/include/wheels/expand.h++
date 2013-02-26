// Wheels - various C++ utilities
//
// Written in 2012 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

// Macro to expand parameter packs in expressions for their side-effects

#ifndef WHEELS_EXPAND_HPP
#define WHEELS_EXPAND_HPP

#include <initializer_list>

// NOTE: not simply a void expression to make clang happy.
//       Should not make much difference
#define WHEELS_EXPAND_SIDE_EFFECTS(...) \
    do { \
        auto x = { ((__VA_ARGS__), void(), 0)... }; \
        (void)x; \
    } while(false)

#endif // WHEELS_EXPAND_HPP


