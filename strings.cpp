#include "base/named_operator.hpp"

#include <iostream>
#include <string>
#include <vector>

namespace op {
    std::vector<std::string> repeat(std::string const& str, std::size_t n) {
        return {n, str};
    }

    std::string join(std::vector<std::string> const& strs,
            std::string const& joiner) {
        std::string result;
        bool first = true;
        for (auto&& str : strs) {
            if (first) first = false;
            else result += joiner;
            result += str;
        }

        return result;
    }
}

auto repeat = base::make_named_operator(op::repeat);
auto join = base::make_named_operator(op::join);

int main() {
    auto result = "Hello" <repeat> 3 <join> ", ";
    std::cout << result << '\n';
}
