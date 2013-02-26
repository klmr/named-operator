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

// Typelists with variadic templates

#ifndef WHEELS_META_TYPELIST_HPP
#define WHEELS_META_TYPELIST_HPP

#include "core.h++"

#include <cstddef>

namespace wheels {
    //! A typelist
    template <typename... T>
    struct typelist {
        using type = typelist;
        using empty = Bool<true>;
    };
    template <typename Head, typename... Tail>
    struct typelist<Head, Tail...> {
        using type = typelist;
        using head = Head;
        using tail = typelist<Tail...>;
        using empty = Bool<false>;
    };
    //! Indices pack
    template <std::size_t... I>
    using Indices = typelist<Const<std::size_t, I>...>;

    //! Indices pack construction
    template <std::size_t Size, typename Acc = typelist<>>
    struct build_indices : typelist<> {};
    template <std::size_t Size, std::size_t... Acc>
    struct build_indices<Size, Indices<Acc...>> : build_indices<Size-1, Indices<Size, Acc...>> {};
    template <std::size_t Size>
    using BuildIndices = Invoke<build_indices<Size>>;
} // namespace wheels

#endif // WHEELS_META_TYPELIST_HPP

