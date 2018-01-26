#define BOOST_TEST_MODULE test_lru
#include <boost/test/included/unit_test.hpp>
#include <boost/test/test_case_template.hpp>

#include <lru.hpp>

#include <string>

#include <boost/functional/hash.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/copy.hpp>

using namespace functools;

// =====================================================================================================================
BOOST_AUTO_TEST_CASE(lru_zero_capacity)
{
    LRU<int, int> lru(0);
    BOOST_CHECK_EQUAL(lru.capacity(), 0);

    BOOST_CHECK_EQUAL(lru.size(), 0);
    BOOST_CHECK(lru.find(1) == lru.end());

    auto ret = lru.emplace(1, 11);
    BOOST_CHECK_EQUAL(lru.size(), 0);
    BOOST_CHECK(lru.find(1) == lru.end());
    BOOST_CHECK(ret.first == lru.end());
    BOOST_CHECK_EQUAL(ret.second, false);
}

// =====================================================================================================================
BOOST_AUTO_TEST_CASE(lru_emplace)
{
    LRU<int, int> lru(2);
    BOOST_CHECK_EQUAL(lru.capacity(), 2);

    BOOST_CHECK_EQUAL(lru.size(), 0);
    lru.emplace(1, 1);
    BOOST_CHECK_EQUAL(lru.size(), 1);
    lru.emplace(2, 2);
    BOOST_CHECK_EQUAL(lru.size(), 2);
    lru.emplace(3, 3);
    BOOST_CHECK_EQUAL(lru.size(), 2);
}

// =====================================================================================================================
BOOST_AUTO_TEST_CASE(lru_find)
{
    LRU<int, int> lru(2);
    BOOST_CHECK_EQUAL(lru.capacity(), 2);

    BOOST_CHECK_EQUAL(lru.size(), 0);
    BOOST_CHECK(lru.find(1) == lru.end());

    lru.emplace(1, 11);
    BOOST_CHECK_EQUAL(lru.size(), 1);
    BOOST_CHECK(lru.find(1) != lru.end());
    BOOST_CHECK(lru.find(1)->second == 11);

    lru.emplace(2, 22);
    BOOST_CHECK_EQUAL(lru.size(), 2);
    BOOST_REQUIRE(lru.find(1) != lru.end());
    BOOST_CHECK(lru.find(1)->second == 11);
    BOOST_REQUIRE(lru.find(2) != lru.end());
    BOOST_CHECK(lru.find(2)->second == 22);
}

// =====================================================================================================================
BOOST_AUTO_TEST_CASE(lru_evict_1)
{
    LRU<int, int> lru(2);

    lru.emplace(1, 11);
    lru.emplace(2, 22);
    lru.emplace(3, 33);

    BOOST_CHECK_EQUAL(lru.size(), 2);
    BOOST_CHECK(lru.find(1) == lru.end());

    BOOST_REQUIRE(lru.find(2) != lru.end());
    BOOST_CHECK(lru.find(2)->second == 22);
    BOOST_REQUIRE(lru.find(3) != lru.end());
    BOOST_CHECK(lru.find(3)->second == 33);
}

// =====================================================================================================================
BOOST_AUTO_TEST_CASE(lru_evict_2)
{
    LRU<int, int> lru(2);

    lru.emplace(1, 11);
    lru.emplace(2, 22);
    lru.find(1);
    lru.emplace(3, 33);

    BOOST_CHECK_EQUAL(lru.size(), 2);
    BOOST_CHECK(lru.find(2) == lru.end());

    BOOST_REQUIRE(lru.find(1) != lru.end());
    BOOST_CHECK(lru.find(1)->second == 11);
    BOOST_REQUIRE(lru.find(3) != lru.end());
    BOOST_CHECK(lru.find(3)->second == 33);
}

// =====================================================================================================================
BOOST_AUTO_TEST_CASE(lru_emplace_update)
{
    LRU<int, int> lru(3);

    BOOST_CHECK_EQUAL(lru.capacity(), 3);
    BOOST_CHECK_EQUAL(lru.size(), 0);

    lru.emplace(1, 1);
    BOOST_CHECK_EQUAL(lru.size(), 1);
    BOOST_CHECK(lru.find(1) != lru.end());

    lru.emplace(2, 2);
    BOOST_CHECK_EQUAL(lru.size(), 2);
    BOOST_CHECK(lru.find(1) != lru.end());
    BOOST_CHECK(lru.find(1)->second == 1);
    BOOST_CHECK(lru.find(2) != lru.end());
    BOOST_CHECK(lru.find(2)->second == 2);

    lru.emplace(1, 11);
    BOOST_CHECK_EQUAL(lru.size(), 2);
    BOOST_CHECK(lru.find(1) != lru.end());
    BOOST_CHECK(lru.find(1)->second == 11);
    BOOST_CHECK(lru.find(2) != lru.end());
    BOOST_CHECK(lru.find(2)->second == 2);
}

// =====================================================================================================================
BOOST_AUTO_TEST_CASE(lru_emplace_return_value)
{
    LRU<int, int> lru(3);

    BOOST_CHECK_EQUAL(lru.capacity(), 3);
    BOOST_CHECK_EQUAL(lru.size(), 0);

    auto ret = lru.emplace(1, 11);
    BOOST_CHECK_EQUAL(ret.second, true);
    BOOST_REQUIRE(ret.first != lru.end());
    BOOST_CHECK_EQUAL(ret.first->first, 1);
    BOOST_CHECK_EQUAL(ret.first->second, 11);

    ret = lru.emplace(2, 22);
    BOOST_CHECK_EQUAL(lru.size(), 2);
    BOOST_CHECK_EQUAL(ret.second, true);
    BOOST_REQUIRE(ret.first != lru.end());
    BOOST_CHECK_EQUAL(ret.first->first, 2);
    BOOST_CHECK_EQUAL(ret.first->second, 22);

    ret = lru.emplace(1, 111);
    BOOST_CHECK_EQUAL(lru.size(), 2);
    BOOST_CHECK_EQUAL(ret.second, false);
    BOOST_REQUIRE(ret.first != lru.end());
    BOOST_CHECK_EQUAL(ret.first->first, 1);
    BOOST_CHECK_EQUAL(ret.first->second, 111);
    BOOST_CHECK(lru.find(1) != lru.end());
    BOOST_CHECK(lru.find(1)->second == 111);
    BOOST_CHECK(lru.find(2) != lru.end());
    BOOST_CHECK(lru.find(2)->second == 22);
}

// =====================================================================================================================
BOOST_AUTO_TEST_CASE(lru_emplace_string)
{
    LRU<int, std::string> lru(3);

    BOOST_CHECK_EQUAL(lru.capacity(), 3);
    BOOST_CHECK_EQUAL(lru.size(), 0);

    auto ret = lru.emplace(1, "11");
    BOOST_REQUIRE(ret.first != lru.end());
    BOOST_CHECK_EQUAL(ret.first->second, "11");
    BOOST_REQUIRE(lru.find(1) != lru.end());
    BOOST_CHECK_EQUAL(lru.find(1)->second, "11");

    ret = lru.emplace(2, "22");
    BOOST_REQUIRE(ret.first != lru.end());
    BOOST_CHECK_EQUAL(ret.first->second, "22");
    BOOST_REQUIRE(lru.find(2) != lru.end());
    BOOST_CHECK_EQUAL(lru.find(2)->second, "22");

    ret = lru.emplace(1, "111");
    BOOST_REQUIRE(ret.first != lru.end());
    BOOST_CHECK_EQUAL(ret.first->second, "111");
    BOOST_REQUIRE(lru.find(1) != lru.end());
    BOOST_CHECK_EQUAL(lru.find(1)->second, "111");
}
