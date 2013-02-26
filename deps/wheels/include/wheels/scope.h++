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

// Scope guards

#ifndef WHEELS_SCOPE_HPP
#define WHEELS_SCOPE_HPP

#include "meta.h++"

#include <exception>

namespace wheels {
    namespace scope_detail {
        template <typename Fun>
        class guard {
        public:
            template <typename U,
                      DisableIf<is_related<guard, U>> = _>
            explicit guard(U&& u) : fun(std::forward<U>(u)) {}

            guard(guard&& that)
            : on(that.on)
            , fun(std::move(that.fun)) {
                that.on = false;
            }

            ~guard() {
                if(on) fun();
            }
        private:
            bool on = true;
            Fun fun;
        };
    } // namespace scope_detail

    template <typename Fun,
              typename Guard = scope_detail::guard<Decay<Fun>>>
    Guard finally(Fun&& fun) {
        return Guard { std::forward<Fun>(fun) };
    }
} // namespace wheels

#endif // WHEELS_SCOPE_HPP

