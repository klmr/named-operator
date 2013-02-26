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

// A box that enforces the use of a lock to access a value

#ifndef WHEELS_LAZY_HPP
#define WHEELS_LAZY_HPP

#include "meta.h++"

#include <utility>

namespace wheels {
    //! Evaluates an eager argument
    template <typename T , typename Eager,
              EnableIf<std::is_convertible<Eager, T>> = _>
    T lazy(Eager&& value) {
        return std::forward<Eager>(value);
    }

    //! Evaluates a lazy argument
    template <typename T , typename Lazy,
              DisableIf<std::is_convertible<Lazy, T>> = _>
    T lazy(Lazy&& value) {
        return std::forward<Lazy>(value)();
    }
}

#endif // WHEELS_LAZY_HPP


