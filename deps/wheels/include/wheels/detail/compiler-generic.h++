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

// Generic compiler feature detection and workarounds

#ifndef WHEELS_DETAIL_COMPILER_GENERIC_HPP
#define WHEELS_DETAIL_COMPILER_GENERIC_HPP

#ifndef WHEELS_UNREACHABLE
    #define WHEELS_UNREACHABLE do { throw "the impossible happened!"; } while(false)
#endif

#ifndef WHEELS_WARNING
    #define WHEELS_WARNING(MESSAGE) WHEELS_EXPECT_SEMICOLON
#endif

#endif // WHEELS_DETAIL_COMPILER_GENERIC_HPP

