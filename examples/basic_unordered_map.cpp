#include <iostream>
#include <iomanip>
#include <unordered_map>

#include <cached.hpp>

namespace std
{
    template<>
    struct hash<std::tuple<int, int>>
    {
        size_t operator() (const std::tuple<int, int>& t) const
        { return (std::get<0>(t) << 1) ^ std::get<1>(t); }
    };
}

int func(int x, int y)
{
    return 3 * x*x*x + 5 * y*y + 7 * x + 1;
}

int main()
{
    auto cached_func = functools::make_cached_func<std::unordered_map>(func);

    for (int j = 0; j < 10; ++j)
    {
        const int i = j % 5;
        const int k = 3;
        const int r1 = func(i, k), r2 = cached_func(i, k);

        std::cout << "i: " << i << ", k: " << k
            << ", func(i, k): " << std::setw(3) << r1
            << ", cached(i, k): " << std::setw(3) << r2
            << ", func(i, k) == cached(i, k): " << std::boolalpha << (r1 == r2)
            << std::endl;
    }

    std::cout << "cache size: " << cached_func.cache_size() << std::endl;

    return 0;
}
