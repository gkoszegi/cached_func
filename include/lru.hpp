#ifndef FUNCTOOLS_LRU_HPP_INCLUDED
#define FUNCTOOLS_LRU_HPP_INCLUDED

#include <unordered_map>
#include <list>

namespace functools
{
    /*
     * LRU (Least Recently Used) container
     */
    template <typename Key, typename Value>
    class LRU
    {
        using OrderList = std::list<Key>;
        using Position = typename OrderList::iterator;

        struct CachedItem
        {
            Value value;
            const Position pos;

            operator Value() const { return value; }
        };

        using Items = std::unordered_map<Key, CachedItem>;

        public:
            using key_type = Key;
            using mapped_type = Value;
            using size_type = typename Items::size_type;
            using iterator = typename Items::iterator;
            using const_iterator = typename Items::const_iterator;

            LRU(size_type capacity)
                : mCapacity(capacity)
            {
                mItems.reserve(capacity);
            }

            template <typename TKey, typename TValue>
            auto emplace(TKey&& key, TValue&& value)
            {
                if (mCapacity == 0)
                    return std::make_pair(end(), false);

                auto found = find(key);
                if (found != end())
                {
                    found->second.value = std::forward<TValue>(value);
                    return std::make_pair(found, false);
                }

                if (mOrder.size() == mCapacity)
                {
                    mItems.erase(mOrder.front());
                    mOrder.front() = key;
                    mOrder.splice(mOrder.end(), mOrder, mOrder.begin());
                }
                else
                {
                    mOrder.push_back(key);
                }

                // static_assert(std::is_move_constructible<CachedItem>::value, "");

                return mItems.emplace(
                        std::forward<TKey>(key),
                        CachedItem{std::forward<TValue>(value), --mOrder.end()});
            }

            iterator find(const Key& key)
            {
                auto found = mItems.find(key);
                if (found == mItems.end())
                    return {};

                mOrder.splice(mOrder.end(), mOrder, found->second.pos);
                return found;
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
