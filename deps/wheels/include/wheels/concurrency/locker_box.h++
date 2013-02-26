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

#ifndef WHEELS_CONCURRENCY_LOCKER_BOX_HPP
#define WHEELS_CONCURRENCY_LOCKER_BOX_HPP

#include "../detail/config.h++"

#if WHEELS_HAS_FEATURE(CXX_THREADS)

#include "../meta.h++"

#include <mutex>
#include <utility>
 
namespace wheels {
    //! A single-value container that enforces synchronised access to the value
    template <typename T, typename BasicLockable = std::mutex>
    struct locker_box {
    public:
        //! Type of the contained value
        using value_type = T;
        //! Type of the guarding mutex
        using mutex_type = BasicLockable;

    private:
        template <typename Box>
        struct cookie {
            cookie() = default;
            cookie(Box& b) : b(&b) {}
     
            WithConstOf<Box, value_type>& operator*() const { return b->value; }
            WithConstOf<Box, value_type>* operator->() { return &b->value; }
            cookie& operator++() {
                b = nullptr;
                return *this;
            }
            cookie operator++(int) {
                cookie copy(*this);
                ++*this;
                return copy;
            }
            bool operator==(cookie const& c) { return b == c.b; }
            bool operator!=(cookie const& c) { return b != c.b; }

        private:
            Box* b = nullptr;
        };
        template <typename Box>
        struct open_box {
            open_box(Box& b) : b(b) { b.m.lock(); }
            ~open_box() { b.m.unlock(); }
            cookie<Box> begin() { return { b }; }
            cookie<Box> end() { return {}; }
        private:
            Box& b;
        };

    public:
        //! Creates a new locker box with a default initialized value
        explicit locker_box() : value{} {}
        //! Creates a new locker box by forwarding the arguments to T's construtor
        template <typename... Args>
        explicit locker_box(Args&&... args) : value(std::forward<Args>(args)...) {}

        //! Gain access to the underlying value
        /*! Remarks: Intended for use in range-based for loops
         *! Example:
         *!     for(auto&& x : box.open()) {
         *!         x.do_stuff();
         *!     }
         */
        open_box<locker_box> open() { return { *this }; }
        //! Gain access to the underlying value
        /*! Remarks: acts just like the non-const version, but provides a const view of the value */
        open_box<locker_box const> open() const { return { *this }; }
     
        //! Obtains the mutex used to guard this instance
        mutex_type const& mutex() const { return m; }

    private:
        template <typename Box>
        friend struct open_box;
        template <typename Box>
        friend struct cookie;

        mutable mutex_type m {};
        value_type value;
    };

    //! Applies a function to the contents of a locker_box
    /*! Returns the function's result, if any */
    template <typename Fun, typename T, typename BasicLockable>
    ResultOf<Fun(T&)> apply(Fun&& fun, locker_box<T, BasicLockable>& box) {
        for(auto&& x : box.open()) { return std::forward<Fun>(fun)(x); }
        WHEELS_UNREACHABLE;
    }
    //! Applies a function to the contents of a locker_box
    /*! Returns the function's result, if any */
    template <typename Fun, typename T, typename BasicLockable>
    ResultOf<Fun(T const&)> apply(Fun&& fun, locker_box<T, BasicLockable> const& box) {
        for(auto&& x : box.open()) { return std::forward<Fun>(fun)(x); }
        WHEELS_UNREACHABLE;
    }
}

#endif // WHEELS_HAS_FEATURE(CXX_THREADS)

#endif // WHEELS_CONCURRENCY_LOCKER_BOX_HPP

