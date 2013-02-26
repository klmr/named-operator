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

// Allocator utilities

#ifndef WHEELS_ALLOCATOR_HPP
#define WHEELS_ALLOCATOR_HPP

#include "meta.h++"

#include <cstddef>
#include <memory>

namespace wheels {
    namespace allocator_detail {
        template <typename A>
        struct deallocate {
        public:
            deallocate(A& a, std::size_t n) : a(a), n(n) {}

            void operator()(typename std::allocator_traits<A>::pointer ptr) {
                a.deallocate(ptr, n);
            }

        private:
            A& a;
            std::size_t n;
        };

        template <typename A,
                  typename Pointer = std::unique_ptr<ValueType<A>, deallocate<A>>>
        Pointer allocate(A& a, std::size_t n) {
            return Pointer { a.allocate(n), deallocate<A> { a, n } };
        }
    } // namespace allocator_detail

    //! Allocator-based copier
    template <typename T, typename Alloc>
    struct allocator_cloner {
    public:
        allocator_cloner(Alloc a) : a(std::move(a)) {}

        T* operator()(T* ptr) {
            auto holder = allocator_detail::allocate(a, 1);
            ::new(holder.get()) T(*ptr);
            return holder.release();
        }

    private:
        Alloc a;
    };

    //! Allocator-based deleter
    template <typename T, typename Alloc>
    struct allocator_delete {
    public:
        allocator_delete(Alloc a) : a(std::move(a)) {}

        void operator()(T* ptr) {
            ptr->~T();
            a.deallocate(ptr, 1);
        }

    private:
        Alloc a;
    };
} // namespace wheels

#endif // WHEELS_ALLOCATOR_HPP

