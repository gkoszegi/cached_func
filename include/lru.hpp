#ifndef FUNCTOOLS_LRU_HPP_INCLUDED
#define FUNCTOOLS_LRU_HPP_INCLUDED

#include <list>
#include <type_traits>
#include <unordered_map>

namespace functools
{
    /*
     * LRU (Least Recently Used) container
     */
    template <typename Key, typename Value>
    class LRU
    {
        using OrderList = std::list<Key>;
        using Position = typename OrderList::const_iterator;

        struct CachedItem
        {
            Value value;
            Position pos;
        };

        using Items = std::unordered_map<Key, CachedItem>;

        public:
            using key_type = Key;
            using mapped_type = Value;
            using size_type = typename Items::size_type;

            struct value_type
                : std::pair<const Key&, Value&>
            {
                value_type(const Key& key, Value& value)
                    : std::pair<const Key&, Value&>(key, value)
                {}

                      value_type* operator-> ()       { return this; }
                const value_type* operator-> () const { return this; }
            };

            class iterator
            {
                friend class LRU;

                public:
                          value_type operator*  ()       { return value_type(it->first, it->second.value); }
                    const value_type operator*  () const { return value_type(it->first, it->second.value); }
                          value_type operator-> ()       { return value_type(it->first, it->second.value); }
                    const value_type operator-> () const { return value_type(it->first, it->second.value); }

                    bool operator == (const iterator& that) const { return it == that.it; }
                    bool operator != (const iterator& that) const { return it != that.it; }

                private:
                    iterator() = default;

                    template<typename T>
                    iterator(T&& p)
                      : it(std::forward<T>(p))
                    {}

                    typename Items::iterator it;
            };

            using const_iterator = const iterator;

            LRU(size_type capacity)
                : mCapacity(capacity)
            {
                mItems.reserve(capacity);
            }

            template <typename TKey, typename TValue>
            std::pair<iterator, bool> emplace(TKey&& key, TValue&& value)
            {
                if (mCapacity == 0)
                    return {end(), false};

                auto found = find(key);
                if (found != end())
                {
                    found->second = std::forward<TValue>(value);
                    return {found, false};
                }

                if (mOrder.size() == mCapacity)
                {
                    auto&& node = mItems.extract(mOrder.front());
                    mOrder.front() = key;
                    mOrder.splice(mOrder.end(), mOrder, mOrder.begin());
                    node.key() = std::forward<TKey>(key);
                    node.mapped().value = std::forward<TValue>(value);
                    return {mItems.insert(mItems.begin(), std::move(node)), true};
                }
                else
                {
                    mOrder.push_back(key);
                    auto result = mItems.emplace(
                            std::forward<TKey>(key),
                            CachedItem{std::forward<TValue>(value), --mOrder.end()});
                    return {std::move(result.first), true};
                }
            }

            iterator find(const Key& key)
            {
                auto found = mItems.find(key);
                if (found == mItems.end())
                    return {};

                mOrder.splice(mOrder.end(), mOrder, found->second.pos);
                return {found};
            }

            iterator end() { return {}; }

            const_iterator end() const { return {}; }

            size_type size() const { return mItems.size(); }

            size_type capacity() const { return mCapacity; }

            void clear()
            {
                mItems.clear();
                mOrder.clear();
            }

        private:
            size_type mCapacity;
            OrderList mOrder;
            Items mItems;
    };

    template <size_t capacity>
    struct FixedCapacity
    {
        template <typename Key, typename Value>
        class LRU: public functools::LRU<Key, Value>
        {
            public:
                LRU(): functools::LRU<Key, Value>(capacity) {}
        };
    };
}

#endif
