#include <iostream>
#include <iomanip>
#include <map>

#include <cached.hpp>

int func(int x)
{
    return 3 * x*x*x + 5 * x*x + 7 * x + 1;
}

int main()
{
    auto cached_func = functools::make_cached_func<std::map>(func);

    for (int j = 0; j < 10; ++j)
    {
        const int i = j % 5;
        const int r1 = func(i), r2 = cached_func(i);

        std::cout << "i: " << i
            << ", func(i): " << std::setw(3) << r1
            << ", cached(i): " << std::setw(3) << r2
            << ", func(i) == cached(i): " << std::boolalpha << (r1 == r2)
            << std::endl;
    }

    std::cout << "cache size: " << cached_func.cache_size() << std::endl;

    return 0;
}
