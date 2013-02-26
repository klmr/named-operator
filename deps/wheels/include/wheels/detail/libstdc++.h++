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

// libstdc++ feature detection and workarounds

#ifndef WHEELS_DETAIL_LIBSTDCPP_HPP
#define WHEELS_DETAIL_LIBSTDCPP_HPP

#define WHEELS_STD_TUPLE_LAYOUT_REVERSED

#define WHEELS_ISSUE_INCOMPLETE_IS_CONVERTIBLE

#if defined(_GLIBCXX_HAS_GTHREADS)
#define WHEELS_HAS_FEATURE_CXX_THREADS
#endif

#endif // WHEELS_DETAIL_LIBSTDCPP_HPP

