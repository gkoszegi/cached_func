Cached functions for C++
========================

Caching the results of complex computations is a common task to implement for speeding up applications.
C++ is missing a simple and generic way to do it. For example python has `functools.lru_cache` function decorator
for this task and I'd like to implement a similarly easy to use tool in C++.

Interface
-----
This library provides a template function `make_cached_func<map_type>(function_type f)` which creates an instance of `cached_func` function object with an identical callable interface as the input `f` function has. The `map_type` template parameter can be any template class whith a similar interface to `std::map` or `std::unordered_map` for example `functools::LRU` also implements the necessary interface, so you can use it also as a storage.

Requirements
------------
- For using `cached.hpp` and `lru.hpp` you will need a C++14 capable compiler (I've been using gcc 5.4 for development).
- For the compilation of the unit tests you will need boost (I used the default 1.58 version coming with Ubuntu 16.04).
- The examples only need C++14 and the `std` library, boost is not necessary.

TODO list and future plans
--------------------------
- Do not wrap the parameter list in `std::tuple` for cache keys that consits only of one value.
- Supply the necessary hash functions to be used with `std::unordered_map` without poluting the std namespace.
- Test usabilty with other map types like `boost::flat_map` and `google::dense_hash_map`.

Example
-------

```cpp
#include <iostream>
#include <iomanip>
#include <map>

#include <cached.hpp>
#include <lru.hpp>

int func(int x)
{
    return 3 * x*x*x + 5 * x*x + 7 * x + 1;
}

int main()
{
    using namespace functools;

    auto cached_func = make_cached_func<FixedCapacity<5>::LRU>(func);

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
```
