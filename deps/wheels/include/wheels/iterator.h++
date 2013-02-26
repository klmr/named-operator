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

// Iterator utilities

#ifndef WHEELS_ITERATOR_H
#define WHEELS_ITERATOR_H

namespace wheels {
    template <typename T>
    using Iterator = typename T::iterator;
    template <typename T>
    using ConstIterator = typename T::const_iterator;

    template <typename Container>
    ConstIterator<Container> cbegin(Container const& container) {
        return container.begin();
    }
    template <typename Container>
    ConstIterator<Container> cend(Container const& container) {
        return container.end();
    }
} // namespace wheels

#endif // WHEELS_ITERATOR_H
