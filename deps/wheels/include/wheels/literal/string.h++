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

// std::string literals

#ifndef WHEELS_LITERAL_STRING_HPP
#define WHEELS_LITERAL_STRING_HPP

#include <cstddef>

#include <string>

namespace wheels {
    namespace literal {
        namespace string {
            //! std::string literal
            inline std::string operator"" _s(char const* str, std::size_t n) {
                return { str, n };
            }
            //! std::wstring literal
            inline std::wstring operator"" _s(wchar_t const* str, std::size_t n) {
                return { str, n };
            }
            //! std::u16string literal
            inline std::u16string operator"" _s(char16_t const* str, std::size_t n) {
                return { str, n };
            }
            //! std::u32string literal
            inline std::u32string operator"" _s(char32_t const* str, std::size_t n) {
                return { str, n };
            }
        } // namespace string
    } // namespace literal
} // namespace wheels

#endif // WHEELS_LITERAL_STRING_HPP

