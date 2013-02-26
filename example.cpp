#include "base/named_operator.hpp"
#include "util/io_helpers.hpp"

namespace op {
    std::pair<int, int> divmod(int x, int y) {
        return { x / y, x % y };
    }

    struct append {
        template <typename T>
        std::vector<T> operator ()(std::vector<T> const& vs, T const& v) const {
            auto copy = vs;
            copy.push_back(v);
            return copy;
        }
    };
} // namespace op

auto divmod = base::make_named_operator(op::divmod);
auto append = base::make_named_operator(op::append());

int main() {
    int x = 42;
    int y = 23;
    auto z = x <divmod> y;

    std::cout << z << '\n';

    std::vector<int> as{ 1, 2, 3 };
    as <append>= 4;

    std::cout << as << '\n';
}
