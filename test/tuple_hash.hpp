#ifndef FUNCTOOLS_TEST_TUPLE_HASH_HPP_INCUDED
#define FUNCTOOLS_TEST_TUPLE_HASH_HPP_INCUDED

#include <boost/functional/hash.hpp>
#include <tuple>

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

#endif
