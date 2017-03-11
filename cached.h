#ifndef FUNCTOOLS_CACHED_H_INCLUDED
#define FUNCTOOLS_CACHED_H_INCLUDED

#include <functional> // function, forward
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
            using size_type = typename MapType::size_type;
            using function_type = std::function<ReturnType(ArgTypes...)>;

            cached_func(const function_type& func)
                : mFunc(func)
            {}

            cached_func(function_type&& func)
                : mFunc(std::move(func))
            {}

            ReturnType operator() (ArgTypes... args) const
            {
                auto found = mCache.find(key_type(args...));
                if (found != mCache.end())
                    return found->second;

                ReturnType ret = mFunc(std::forward<ArgTypes>(args)...);
                mCache.emplace(key_type(args...), ret);
                return ret;
            }

            size_type cache_size() const
            { return mCache.size(); }

            void cache_clear()
            { mCache.clear(); }

        private:
            function_type mFunc;
            mutable MapType mCache;
    };

    template <template<typename...>class MapType, typename ReturnType, typename... ArgTypes>
    cached_func<MapType<std::tuple<ArgTypes...>, ReturnType>, ReturnType, ArgTypes...>
    make_cached_func(ReturnType func(ArgTypes...))
    {
        return {func};
    }

    template <template<typename...>class MapType, typename ReturnType, typename... ArgTypes>
    cached_func<MapType<std::tuple<ArgTypes...>, ReturnType>, ReturnType, ArgTypes...>
    make_cached_func(const std::function<ReturnType(ArgTypes...)>& func)
    {
        return {func};
    }

    template <template<typename...>class MapType, typename ReturnType, typename... ArgTypes>
    cached_func<MapType<std::tuple<ArgTypes...>, ReturnType>, ReturnType, ArgTypes...>
    make_cached_func(std::function<ReturnType(ArgTypes...)>&& func)
    {
        return {std::move(func)};
    }
}

#endif
