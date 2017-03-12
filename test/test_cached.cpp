#define BOOST_TEST_MODULE test_cached
#include <boost/test/included/unit_test.hpp>
#include <boost/test/test_case_template.hpp>

#include <cached.hpp>

#include <algorithm>
#include <map>
#include <string>
#include <unordered_map>

#include <boost/functional/hash.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/copy.hpp>

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
BOOST_AUTO_TEST_SUITE(test_parameter_counts)

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
BOOST_AUTO_TEST_SUITE(test_creation)

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

BOOST_AUTO_TEST_SUITE_END()

// =====================================================================================================================
BOOST_AUTO_TEST_SUITE(test_types_and_parameters)

BOOST_AUTO_TEST_CASE(cache_should_not_store_function_params_as_references)
{
    std::string str1 = "orig", str2 = "orig";
    auto cachedFunc = make_cached_func<std::map>(func3);

    BOOST_CHECK_EQUAL(func3(1, 'X', str1), cachedFunc(1, 'X', str1));
    BOOST_CHECK_EQUAL(func3(2, 'Y', str1), cachedFunc(2, 'Y', str1));
    BOOST_CHECK_EQUAL(cachedFunc.cache_size(), 2);

    BOOST_CHECK_EQUAL(func3(1, 'X', str2), cachedFunc(1, 'X', str2));
    BOOST_CHECK_EQUAL(func3(2, 'Y', str2), cachedFunc(2, 'Y', str2));
    BOOST_CHECK_EQUAL(cachedFunc.cache_size(), 2);

    str1 = "changed";
    BOOST_CHECK_EQUAL(func3(1, 'X', str1), cachedFunc(1, 'X', str1));
    BOOST_CHECK_EQUAL(func3(2, 'Y', str1), cachedFunc(2, 'Y', str1));
    BOOST_CHECK_EQUAL(cachedFunc.cache_size(), 4);

    str2 = "changed";
    BOOST_CHECK_EQUAL(func3(1, 'X', str2), cachedFunc(1, 'X', str2));
    BOOST_CHECK_EQUAL(func3(2, 'Y', str2), cachedFunc(2, 'Y', str2));
    BOOST_CHECK_EQUAL(cachedFunc.cache_size(), 4);
}

BOOST_AUTO_TEST_CASE(std_map_noncapturing_lambda)
{
    auto lambda = [] (int p) { return 10 + p; };
    // non-capturing lambda is convertible to function ptr (operator+ just forces it)
    auto cachedLambda = make_cached_func<std::map>(+lambda);
    BOOST_CHECK_EQUAL(lambda(3), cachedLambda(3));
    BOOST_CHECK_EQUAL(lambda(3), cachedLambda(3));
    BOOST_CHECK_EQUAL(lambda(5), cachedLambda(5));
}

BOOST_AUTO_TEST_CASE(std_map_refcapturing_lambda)
{
    int a = 5;
    auto lambda = [&] (int p) { return a + p; };
    auto func = std::function<int(int)>(lambda);
    auto cachedLambda = make_cached_func<std::map>(func);
    a = 10;
    BOOST_CHECK_EQUAL(lambda(3), 13);
    BOOST_CHECK_EQUAL(lambda(3), cachedLambda(3));
    BOOST_CHECK_EQUAL(lambda(3), cachedLambda(3));
    BOOST_CHECK_EQUAL(lambda(4), cachedLambda(4));
    a = 20;
    BOOST_CHECK_EQUAL(lambda(3), 23);
    BOOST_CHECK_EQUAL(cachedLambda(3), 13); // cache hit
    BOOST_CHECK_EQUAL(cachedLambda(4), 14); // cache hit
    BOOST_CHECK_EQUAL(cachedLambda(5), 25); // cache miss

}

BOOST_AUTO_TEST_SUITE_END()

// =====================================================================================================================
BOOST_AUTO_TEST_SUITE(usecases_for_cached_func)

BOOST_AUTO_TEST_CASE(converted_to_std_function)
{
    std::function<unsigned(unsigned)>
        func = [] (unsigned p) { return 10 * p; },
        cachedFunc = make_cached_func<std::map>(func);

    BOOST_CHECK_EQUAL(func(7), cachedFunc(7));
    BOOST_CHECK_EQUAL(func(0), cachedFunc(0));
}

BOOST_AUTO_TEST_CASE(used_in_std_transform_with_1_input_range)
{
    auto cachedFunc = make_cached_func<std::unordered_map>(func1);
    const std::vector<int> input = {1, 1, 2, 1, 2, 3, 4};
    std::vector<int> outOrig(input.size()), outCached(input.size());

    std::transform(input.begin(), input.end(), outOrig.begin(), func1);
    std::transform(input.begin(), input.end(), outCached.begin(), cachedFunc);

    BOOST_CHECK_EQUAL_COLLECTIONS(
            outOrig.begin(), outOrig.end(),
            outCached.begin(), outCached.end());
}

BOOST_AUTO_TEST_CASE(used_in_std_transform_with_2_input_ranges)
{
    auto cachedFunc = make_cached_func<std::unordered_map>(func2);
    const std::vector<int> input1 = {1, 1, 2, 1, 2, 3, 4, 1, 1, 5};
    const std::vector<int> input2 = {1, 1, 1, 1, 1, 1, 1, 2, 2, 2};
    std::vector<int> outOrig(input1.size()), outCached(input1.size());

    std::transform(input1.begin(), input1.end(), input2.begin(), outOrig.begin(), func2);
    std::transform(input1.begin(), input1.end(), input2.begin(), outCached.begin(), cachedFunc);

    BOOST_CHECK_EQUAL_COLLECTIONS(
            outOrig.begin(), outOrig.end(),
            outCached.begin(), outCached.end());
}

BOOST_AUTO_TEST_CASE(used_in_boost_range_transform)
{
    auto cachedFunc = make_cached_func<std::unordered_map>(func1);
    const std::vector<int> input = {1, 1, 2, 1, 2, 3, 4};
    std::vector<int> outOrig(input.size()), outCached(input.size());

    using boost::adaptors::transform;
    boost::copy(transform(input, func1), outOrig.begin());
    boost::copy(transform(input, cachedFunc), outCached.begin());

    BOOST_CHECK_EQUAL_COLLECTIONS(
            outOrig.begin(), outOrig.end(),
            outCached.begin(), outCached.end());
}

BOOST_AUTO_TEST_CASE(used_in_boost_range_transformed)
{
    auto cachedFunc = make_cached_func<std::unordered_map>(func1);
    const std::vector<int> input = {1, 1, 2, 1, 2, 3, 4};
    std::vector<int> outOrig(input.size()), outCached(input.size());

    using boost::adaptors::transformed;
    boost::copy(input | transformed(func1), outOrig.begin());
    boost::copy(input | transformed(cachedFunc), outCached.begin());

    BOOST_CHECK_EQUAL_COLLECTIONS(
            outOrig.begin(), outOrig.end(),
            outCached.begin(), outCached.end());
}

BOOST_AUTO_TEST_SUITE_END()
