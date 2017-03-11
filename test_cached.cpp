#define BOOST_TEST_MODULE test_cached
#include <boost/test/included/unit_test.hpp>
#include <boost/test/test_case_template.hpp>

#include "cached.h"

#include <string>
#include <map>
#include <unordered_map>
#include <boost/functional/hash.hpp>

using namespace functools;

namespace std
{
    template<typename T>
    struct hash<std::tuple<T>>
    {
        size_t operator() (const std::tuple<T>& tup) const
        {
            return std::hash<T>()(std::get<0>(tup));
        }
    };
    
    template<typename T1, typename T2>
    struct hash<std::tuple<T1, T2>>
    {
        size_t operator() (const std::tuple<T1, T2>& tup) const
        {
            size_t h = 0;
            boost::hash_combine(h, std::get<0>(tup));
            boost::hash_combine(h, std::get<1>(tup));
            return h;
        }
    };
}

namespace
{
    int func1(int n)
    { return 100 + n; }

    int func2(int n, int m)
    { return n * 10 + m; }

    std::string func3(int n, const std::string& s)
    { return s + std::string(n, 'X'); }
}

BOOST_AUTO_TEST_SUITE(functional)

BOOST_AUTO_TEST_CASE(std_map_func1)
{
    auto cached_func1 = make_cached_func<std::map>(func1);
    BOOST_CHECK_EQUAL(cached_func1.cache_size(), 0);
    BOOST_CHECK_EQUAL(func1(1), cached_func1(1));
    BOOST_CHECK_EQUAL(cached_func1.cache_size(), 1);
    BOOST_CHECK_EQUAL(func1(1), cached_func1(1));
    BOOST_CHECK_EQUAL(cached_func1.cache_size(), 1);
    BOOST_CHECK_EQUAL(func1(2), cached_func1(2));
    BOOST_CHECK_EQUAL(cached_func1.cache_size(), 2);
    BOOST_CHECK_EQUAL(func1(1), cached_func1(1));
    BOOST_CHECK_EQUAL(cached_func1.cache_size(), 2);
    BOOST_CHECK_EQUAL(func1(2), cached_func1(2));
    BOOST_CHECK_EQUAL(cached_func1.cache_size(), 2);
}

BOOST_AUTO_TEST_CASE(std_unordered_map_func1)
{
    auto cached_func1 = make_cached_func<std::unordered_map>(func1);
    BOOST_CHECK_EQUAL(func1(1), cached_func1(1));
    BOOST_CHECK_EQUAL(func1(1), cached_func1(1));
    BOOST_CHECK_EQUAL(func1(2), cached_func1(2));
    BOOST_CHECK_EQUAL(func1(1), cached_func1(1));
    BOOST_CHECK_EQUAL(func1(2), cached_func1(2));
}

BOOST_AUTO_TEST_CASE(std_map_func2)
{
    auto cached_func2 = make_cached_func<std::map>(func2);
    BOOST_CHECK_EQUAL(func2(1, 2), cached_func2(1, 2));
    BOOST_CHECK_EQUAL(func2(1, 2), cached_func2(1, 2));
    BOOST_CHECK_EQUAL(func2(2, 3), cached_func2(2, 3));
    BOOST_CHECK_EQUAL(func2(1, 2), cached_func2(1, 2));
    BOOST_CHECK_EQUAL(func2(2, 3), cached_func2(2, 3));
}

BOOST_AUTO_TEST_CASE(std_unordered_map_func2)
{
    auto cached_func2 = make_cached_func<std::unordered_map>(func2);
    BOOST_CHECK_EQUAL(func2(1, 2), cached_func2(1, 2));
    BOOST_CHECK_EQUAL(func2(1, 2), cached_func2(1, 2));
    BOOST_CHECK_EQUAL(func2(2, 3), cached_func2(2, 3));
    BOOST_CHECK_EQUAL(func2(1, 2), cached_func2(1, 2));
    BOOST_CHECK_EQUAL(func2(2, 3), cached_func2(2, 3));
}

BOOST_AUTO_TEST_CASE(std_map_func3)
{
    auto cached_func3 = make_cached_func<std::map>(func3);
    BOOST_CHECK_EQUAL(func3(1, "str"), cached_func3(1, "str"));
    BOOST_CHECK_EQUAL(func3(1, "str"), cached_func3(1, "str"));
    BOOST_CHECK_EQUAL(func3(2, "str"), cached_func3(2, "str"));
    BOOST_CHECK_EQUAL(func3(1, "str"), cached_func3(1, "str"));
    BOOST_CHECK_EQUAL(func3(2, "str"), cached_func3(2, "str"));
}

BOOST_AUTO_TEST_CASE(std_unordered_map_func3)
{
    auto cached_func3 = make_cached_func<std::unordered_map>(func3);
    BOOST_CHECK_EQUAL(func3(1, "str"), cached_func3(1, "str"));
    BOOST_CHECK_EQUAL(func3(1, "str"), cached_func3(1, "str"));
    BOOST_CHECK_EQUAL(func3(2, "str"), cached_func3(2, "str"));
    BOOST_CHECK_EQUAL(func3(1, "str"), cached_func3(1, "str"));
    BOOST_CHECK_EQUAL(func3(2, "str"), cached_func3(2, "str"));
}

BOOST_AUTO_TEST_CASE(std_map_refcapturing_lambda)
{
    int a = 5;
    auto lambda = [&] (int p) { return a + p; };
    auto func = std::function<int(int)>(lambda);
    auto cached_lambda = make_cached_func<std::map>(func);
    a = 10;
    BOOST_CHECK_EQUAL(lambda(3), 13);
    BOOST_CHECK_EQUAL(lambda(3), cached_lambda(3));
    BOOST_CHECK_EQUAL(lambda(3), cached_lambda(3));
    BOOST_CHECK_EQUAL(lambda(4), cached_lambda(4));
    a = 20;
    BOOST_CHECK_EQUAL(lambda(3), 23);
    BOOST_CHECK_EQUAL(cached_lambda(3), 13);
    BOOST_CHECK_EQUAL(cached_lambda(4), 14);
    BOOST_CHECK_EQUAL(cached_lambda(5), 25);

}

BOOST_AUTO_TEST_SUITE_END()
