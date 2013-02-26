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

// Pointer named parameters

#ifndef WHEELS_DETAIL_POINTER_PARAM_HPP
#define WHEELS_DETAIL_POINTER_PARAM_HPP

#include "../named_param.h++"

namespace wheels {
    inline namespace names {
        WHEELS_DEFINE_NAMED_PARAMETER(raw);
        WHEELS_DEFINE_NAMED_PARAMETER(cloner);
        WHEELS_DEFINE_NAMED_PARAMETER(deleter);
    } // namespace names
} // namespace wheels

#endif // WHEELS_DETAIL_POINTER_PARAM_HPP

