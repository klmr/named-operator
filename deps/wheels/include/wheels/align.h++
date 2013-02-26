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

// Alignment tool utilities

#ifndef WHEELS_ALIGN_HPP
#define WHEELS_ALIGN_HPP

//#include "meta.h++"

#include <cstddef>
#include <memory>
#include <tuple>

namespace wheels {
    //! alternative interface for align
    //! Usage: std::tie(aligned, remaining) = align(16, size, unaligned, total);
    inline std::tuple<void*, std::size_t> align(std::size_t alignment, std::size_t size, void* ptr, std::size_t space) {
        auto r = std::align(alignment, size, ptr, space);
        return std::make_tuple(r, space);
    }
} // namespace wheels

#endif // WHEELS_ALLOCATOR_HPP



