#ifndef FUNCTOOLS_CACHED_HPP_INCLUDED
#define FUNCTOOLS_CACHED_HPP_INCLUDED

#include <functional> // function, forward
#include <type_traits> // remove_reference
#include <utility> // tuple

namespace functools
{
    template<
        class MapType,
        typename ReturnType,
        typename... ArgTypes>
    class cached_func
    {
        public:
            using key_type = typename MapType::key_type;
            using mapped_type = typename MapType::mapped_type;
            using result_type = mapped_type;
            using size_type = typename MapType::size_type;
            using function_type = std::function<ReturnType(ArgTypes...)>;

            cached_func(const function_type& func)
                : mFunc(func)
            {}

            cached_func(function_type&& func)
                : mFunc(std::move(func))
            {}

            result_type operator() (ArgTypes... args) const
            {
                key_type key(args...);
                auto found = mCache.find(key);
                if (found != mCache.end())
                    return found->second;

                result_type ret = mFunc(std::forward<ArgTypes>(args)...);
                mCache.emplace(std::move(key), ret);
                return std::move(ret);
            }

            size_type cache_size() const
            { return mCache.size(); }

            void cache_clear()
            { mCache.clear(); }

        private:
            function_type mFunc;
            mutable MapType mCache;
    };

    template <
        template<typename...> class MapType,
        typename ReturnType,
        typename... ArgTypes>
    cached_func<
        MapType<
            std::tuple<std::remove_reference_t<ArgTypes>...>,
            std::remove_reference_t<ReturnType> >,
        ReturnType,
        ArgTypes... >
    make_cached_func(ReturnType func(ArgTypes...))
    {
        return {func};
    }

    template <
        template<typename...> class MapType,
        typename ReturnType,
        typename... ArgTypes>
    cached_func<
        MapType<
            std::tuple<std::remove_reference_t<ArgTypes>...>,
            std::remove_reference_t<ReturnType> >,
        ReturnType,
        ArgTypes... >
    make_cached_func(std::function<ReturnType(ArgTypes...)> func)
    {
        return {std::move(func)};
    }
}

#endif
