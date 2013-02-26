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

// Timing facilities

#ifndef WHEELS_STOPWATCH_HPP
#define WHEELS_STOPWATCH_HPP

#include <cassert>

#include <chrono>
#include <utility>

namespace wheels {
    //! A class for measuring time
    template <typename Clock>
    class basic_stopwatch {
    public:
        using rep = typename Clock::rep;
        using period = typename Clock::period;
        using duration = typename Clock::duration;
        using time_point = typename Clock::time_point;

        //! Creates a new stopwatch
        /*! Postcondition: is_running() == false && elapsed == duration::zero() */
        basic_stopwatch() = default;

        //! Starts or resumes measuring time
        /*! Precondition: is_running() == false
         *! Postcondition: is_running() == true
         */
        void start() {
            assert(!running);
            running = true;
            p = Clock::now();
            last_lap = p;
        }
        //! Stops measuring time
        /*! Precondition: is_running() == true
         *! Postcondition: is_running() == false
         */
        void stop() {
            assert(running);
            d += Clock::now() - p;
            running = false;
        }

        //! Tests if the stopwatch is currently measuring time
        /*! Returns: true if the stopwatch is measuring time; false otherwise */
        bool is_running() const {
            return running;
        }

        //! Restarts the measurement
        /*! Precondition: is_running() == true
         *! Postcondition: is_running() == true && elapsed() == duration::zero()
         */
        void restart() {
            assert(running);
            reset();
            start();
        }
        //! Resets the stopwatch
        /*! Postcondition: is_running() == false && elapsed() == duration::zero() */
        void reset() {
            d = std::chrono::duration_values<duration>::zero();
            running = false;
        }

        //! Obtains the current measurement
        /*! Returns: the total time elapsed during the current (if running) or last measurement (if not running) */
        duration elapsed() const {
            if(running) return d + Clock::now() - p;
            else return d;
        }
        //! Obtains the lap time
        /*! Returns: the total time elapsed since the last call to lap()
         *!          or since the start of the measurement if it is the first call to lap() since the start
         */
        duration lap() {
            assert(running);
            auto new_lap = Clock::now();
            auto lap_time = new_lap - last_lap;
            last_lap = new_lap;
            return lap_time;
        }

        //! Creates and starts a new stopwatch
        /*! Returns: a new stopwatch with is_running() == true
         */
        static basic_stopwatch start_new() {
            basic_stopwatch sw;
            sw.start();
            return sw;
        }

    private:
        time_point p {};
        time_point last_lap {};
        duration d = std::chrono::duration_values<duration>::zero();
        bool running = false;
    };

    //! A stopwatch with the high-resolution clock
    using stopwatch = basic_stopwatch<std::chrono::high_resolution_clock>;

    //! Execution of a function and measures the elapsed time
    /*! Returns: the total time elapsed in running fun */
    template <typename Clock = std::chrono::high_resolution_clock, typename Fun>
    typename basic_stopwatch<Clock>::duration time_execution(Fun&& fun) {
        basic_stopwatch<Clock> sw;

        sw.start();
        std::forward<Fun>(fun)();
        sw.stop();

        return sw.elapsed();
    }
} // namespace wheels

#endif // WHEELS_STOPWATCH_HPP

