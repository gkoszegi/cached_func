#define BOOST_TEST_MODULE test_cached
#include <boost/test/included/unit_test.hpp>
#include <boost/test/test_case_template.hpp>

#include "cached.h"

#include <string>
#include <map>
#include <unordered_map>
#include <boost/functional/hash.hpp>

using namespace functools;

// =====================================================================================================================
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
    struct hash<std::pair<T1, T2>>
    {
        size_t operator() (const std::pair<T1, T2>& p) const
        {
            size_t h = 0;
            boost::hash_combine(h, p.first);
            boost::hash_combine(h, p.second);
            return h;
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

    template<typename T1, typename T2, typename T3>
    struct hash<std::tuple<T1, T2, T3>>
    {
        size_t operator() (const std::tuple<T1, T2, T3>& tup) const
        {
            size_t h = 0;
            boost::hash_combine(h, std::get<0>(tup));
            boost::hash_combine(h, std::get<1>(tup));
            boost::hash_combine(h, std::get<2>(tup));
            return h;
        }
    };
}

// =====================================================================================================================
namespace
{
    int func1(int n)
    { return 100 + n; }

    int func2(int n, int m)
    { return n * 10 + m; }

    std::string func3(int n, char c, const std::string& s)
    { return s + std::string(n, c); }

    using TestCachedFunc1 = boost::mpl::list<
        cached_func<std::map<int, int>, int, int>,
        cached_func<std::map<std::tuple<int>, int>, int, int>,
        cached_func<std::unordered_map<int, int>, int, int>,
        cached_func<std::unordered_map<std::tuple<int>, int>, int, int> >;

    using TestCachedFunc2 = boost::mpl::list<
        cached_func<std::map<std::tuple<int, int>, int>, int, int, int>,
        cached_func<std::map<std::pair<int, int>, int>, int, int, int>,
        cached_func<std::unordered_map<std::tuple<int, int>, int>, int, int, int>,
        cached_func<std::unordered_map<std::pair<int, int>, int>, int, int, int> >;

    using TestCachedFunc3 = boost::mpl::list<
        cached_func<
            std::map<std::tuple<int, char, std::string>, std::string>,
            std::string,
            int, char, std::string>,
        cached_func<
            std::unordered_map<std::tuple<int, char, std::string>, std::string>,
            std::string,
            int, char, std::string>
    >;
}

// =====================================================================================================================
BOOST_AUTO_TEST_SUITE(test_cached_func)

BOOST_AUTO_TEST_CASE_TEMPLATE(function_with_one_param, TCachedFunc, TestCachedFunc1)
{
    auto cachedFunc = TCachedFunc(func1);
    BOOST_CHECK_EQUAL(cachedFunc.cache_size(), 0);
    BOOST_CHECK_EQUAL(func1(1), cachedFunc(1));
    BOOST_CHECK_EQUAL(cachedFunc.cache_size(), 1);
    BOOST_CHECK_EQUAL(func1(1), cachedFunc(1));
    BOOST_CHECK_EQUAL(cachedFunc.cache_size(), 1);
    BOOST_CHECK_EQUAL(func1(2), cachedFunc(2));
    BOOST_CHECK_EQUAL(cachedFunc.cache_size(), 2);
    BOOST_CHECK_EQUAL(func1(1), cachedFunc(1));
    BOOST_CHECK_EQUAL(cachedFunc.cache_size(), 2);
    BOOST_CHECK_EQUAL(func1(2), cachedFunc(2));
    BOOST_CHECK_EQUAL(cachedFunc.cache_size(), 2);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(function_with_two_params, TCachedFunc, TestCachedFunc2)
{
    auto cachedFunc = TCachedFunc(func2);
    BOOST_CHECK_EQUAL(cachedFunc.cache_size(), 0);
    BOOST_CHECK_EQUAL(func2(1, 2), cachedFunc(1, 2));
    BOOST_CHECK_EQUAL(cachedFunc.cache_size(), 1);
    BOOST_CHECK_EQUAL(func2(1, 2), cachedFunc(1, 2));
    BOOST_CHECK_EQUAL(cachedFunc.cache_size(), 1);
    BOOST_CHECK_EQUAL(func2(2, 3), cachedFunc(2, 3));
    BOOST_CHECK_EQUAL(cachedFunc.cache_size(), 2);
    BOOST_CHECK_EQUAL(func2(1, 2), cachedFunc(1, 2));
    BOOST_CHECK_EQUAL(cachedFunc.cache_size(), 2);
    BOOST_CHECK_EQUAL(func2(2, 3), cachedFunc(2, 3));
    BOOST_CHECK_EQUAL(cachedFunc.cache_size(), 2);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(function_with_three_params, TCachedFunc, TestCachedFunc3)
{
    auto cachedFunc = TCachedFunc(func3);
    BOOST_CHECK_EQUAL(cachedFunc.cache_size(), 0);
    BOOST_CHECK_EQUAL(func3(1, 'X', "str"), cachedFunc(1, 'X', "str"));
    BOOST_CHECK_EQUAL(cachedFunc.cache_size(), 1);
    BOOST_CHECK_EQUAL(func3(1, 'X', "str"), cachedFunc(1, 'X', "str"));
    BOOST_CHECK_EQUAL(cachedFunc.cache_size(), 1);
    BOOST_CHECK_EQUAL(func3(2, 'Y', "str"), cachedFunc(2, 'Y', "str"));
    BOOST_CHECK_EQUAL(cachedFunc.cache_size(), 2);
    BOOST_CHECK_EQUAL(func3(1, 'X', "str"), cachedFunc(1, 'X', "str"));
    BOOST_CHECK_EQUAL(cachedFunc.cache_size(), 2);
    BOOST_CHECK_EQUAL(func3(2, 'Y', "str"), cachedFunc(2, 'Y', "str"));
    BOOST_CHECK_EQUAL(cachedFunc.cache_size(), 2);
    BOOST_CHECK_EQUAL(func3(2, 'Z', "str"), cachedFunc(2, 'Z', "str"));
    BOOST_CHECK_EQUAL(cachedFunc.cache_size(), 3);
}

BOOST_AUTO_TEST_SUITE_END()

// =====================================================================================================================
BOOST_AUTO_TEST_SUITE(test_make_cached_func)

BOOST_AUTO_TEST_CASE(std_map_func1)
{
    auto cachedFunc = make_cached_func<std::map>(func1);
    BOOST_CHECK_EQUAL(func1(1), cachedFunc(1));
    BOOST_CHECK_EQUAL(func1(1), cachedFunc(1));
    BOOST_CHECK_EQUAL(func1(2), cachedFunc(2));
    BOOST_CHECK_EQUAL(func1(1), cachedFunc(1));
    BOOST_CHECK_EQUAL(func1(2), cachedFunc(2));
}

BOOST_AUTO_TEST_CASE(std_unordered_map_func1)
{
    auto cachedFunc = make_cached_func<std::unordered_map>(func1);
    BOOST_CHECK_EQUAL(func1(1), cachedFunc(1));
    BOOST_CHECK_EQUAL(func1(1), cachedFunc(1));
    BOOST_CHECK_EQUAL(func1(2), cachedFunc(2));
    BOOST_CHECK_EQUAL(func1(1), cachedFunc(1));
    BOOST_CHECK_EQUAL(func1(2), cachedFunc(2));
}

BOOST_AUTO_TEST_CASE(std_map_func2)
{
    auto cachedFunc = make_cached_func<std::map>(func2);
    BOOST_CHECK_EQUAL(func2(1, 2), cachedFunc(1, 2));
    BOOST_CHECK_EQUAL(func2(1, 2), cachedFunc(1, 2));
    BOOST_CHECK_EQUAL(func2(2, 3), cachedFunc(2, 3));
    BOOST_CHECK_EQUAL(func2(1, 2), cachedFunc(1, 2));
    BOOST_CHECK_EQUAL(func2(2, 3), cachedFunc(2, 3));
}

BOOST_AUTO_TEST_CASE(std_unordered_map_func2)
{
    auto cachedFunc = make_cached_func<std::unordered_map>(func2);
    BOOST_CHECK_EQUAL(func2(1, 2), cachedFunc(1, 2));
    BOOST_CHECK_EQUAL(func2(1, 2), cachedFunc(1, 2));
    BOOST_CHECK_EQUAL(func2(2, 3), cachedFunc(2, 3));
    BOOST_CHECK_EQUAL(func2(1, 2), cachedFunc(1, 2));
    BOOST_CHECK_EQUAL(func2(2, 3), cachedFunc(2, 3));
}

BOOST_AUTO_TEST_CASE(std_map_func3)
{
    auto cachedFunc = make_cached_func<std::map>(func3);
    BOOST_CHECK_EQUAL(func3(1, 'X', "str"), cachedFunc(1, 'X', "str"));
    BOOST_CHECK_EQUAL(func3(1, 'X', "str"), cachedFunc(1, 'X', "str"));
    BOOST_CHECK_EQUAL(func3(2, 'Y', "str"), cachedFunc(2, 'Y', "str"));
    BOOST_CHECK_EQUAL(func3(1, 'X', "str"), cachedFunc(1, 'X', "str"));
    BOOST_CHECK_EQUAL(func3(2, 'Y', "str"), cachedFunc(2, 'Y', "str"));
}

BOOST_AUTO_TEST_CASE(std_unordered_map_func3)
{
    auto cachedFunc = make_cached_func<std::unordered_map>(func3);
    BOOST_CHECK_EQUAL(func3(1, 'X', "str"), cachedFunc(1, 'X', "str"));
    BOOST_CHECK_EQUAL(func3(1, 'X', "str"), cachedFunc(1, 'X', "str"));
    BOOST_CHECK_EQUAL(func3(2, 'Y', "str"), cachedFunc(2, 'Y', "str"));
    BOOST_CHECK_EQUAL(func3(1, 'X', "str"), cachedFunc(1, 'X', "str"));
    BOOST_CHECK_EQUAL(func3(2, 'Y', "str"), cachedFunc(2, 'Y', "str"));
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
    BOOST_CHECK_EQUAL(cached_lambda(3), 13); // cache hit
    BOOST_CHECK_EQUAL(cached_lambda(4), 14); // cache hit
    BOOST_CHECK_EQUAL(cached_lambda(5), 25); // cache miss

}

BOOST_AUTO_TEST_SUITE_END()
