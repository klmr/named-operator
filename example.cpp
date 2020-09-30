#include "base/named_operator.hpp"
#include "util/io_helpers.hpp"

#include <algorithm>
#include <iomanip>
#include <vector>
#include <utility>

namespace op {
    std::pair<int, int> divmod(int x, int y) {
        return { x / y, x % y };
    }//If we don't want to to write std:: everytime then we can include "using namespace std"

    struct append {
        template <typename T>
        std::vector<T> operator ()(std::vector<T> const& vs, T const& v) const {
            auto copy(vs);
            copy.push_back(v);
            return copy;
        }
    };
} // namespace op

auto divmod = base::make_named_operator(op::divmod);
auto append = base::make_named_operator(op::append());
auto in = base::make_named_operator(
    [](int i, std::vector<int> const& x) {
        return std::find(begin(x), end(x), i) != end(x);
    });

int main() {
    int x = 42;
    int y = 23;
    auto z = x <divmod> y;

    std::cout << z << '\n';

    std::vector<int> vec{ 1, 2, 3 };
    vec <append>= 4;

    std::cout << std::boolalpha;
    std::cout << "3 in " << vec << ": " << (3 <in> vec) << '\n'
              << "5 in " << vec << ": " << (5 <in> vec) << '\n';
}
