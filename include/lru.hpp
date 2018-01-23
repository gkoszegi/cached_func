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
            Position pos;

            operator Value() const
            { return value; }
        };

        using Items = std::unordered_map<Key, CachedItem>;

        public:
            using key_type = Key;
            using mapped_type = Value;
            using size_type = typename Items::size_type;
            using const_iterator = typename Items::const_iterator;

            LRU(size_type capacity)
                : mCapacity(capacity)
            {
                mItems.reserve(capacity);
            }

            template <typename TKey, typename TValue>
            void emplace(TKey&& key, TValue&& value)
            {
                if (mCapacity == 0)
                    return;

                if (mOrder.size() == mCapacity)
                    evict_least_recent();

                mOrder.push_back(key);
                mItems.emplace(
                        std::forward<TKey>(key),
                        CachedItem{std::forward<TValue>(value), --mOrder.end()});
            }

            const_iterator find(const Key& key)
            {
                auto found = mItems.find(key);
                if (found == mItems.end())
                    return {};

                mOrder.splice(mOrder.end(), mOrder, found->second.pos);
                return found;
            }

            const_iterator end() const
            {
                return {};
            }

            size_type size() const
            {
                return mItems.size();
            }

            size_type capacity() const
            {
                return mCapacity;
            }

            void clear()
            {
                mItems.clear();
                mOrder.clear();
            }

        private:
            void evict_least_recent()
            {
                mItems.erase(mOrder.front());
                mOrder.pop_front();
            }

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
