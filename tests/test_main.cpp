#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "PriceLevel.hpp"
#include "OrderBook.hpp"

// add two orders at the same price level sum together to the quantity
TEST_CASE("PriceLevel aggregates same-price quantity") {
    PriceLevel p {};
    int quant1 {15};
    int quant2 {20};
    Order a {1, Side::Buy, 101, quant1};
    Order b {1, Side::Buy, 101, quant2};

    p.add(a);
    p.add(b);
    CHECK(p.total_quantity() == (quant1 + quant2));
}

// best_bid and best_ask return std::nullopt
TEST_CASE("Empty Book has no best bid or best ask") {
    OrderBook book {};

    CHECK(book.best_ask() == std::nullopt);
    CHECK(book.best_bid() == std::nullopt);
}

TEST_CASE("best_bid is the highest resting bid") {
    OrderBook book {};
    Order o1 {1, Side::Buy, 103, 5};
    Order o2 {2, Side::Buy, 109, 6};
    Order o3 {3, Side::Buy, 106, 3};

    book.add_order(o1);
    book.add_order(o2);
    book.add_order(o3);

    std::optional<int> bid = book.best_bid();
    REQUIRE(bid.has_value());
    CHECK(*bid == 109);
}

TEST_CASE("best_ask is the lowest resting ask") {
    OrderBook book {};
    Order o1 {1, Side::Sell, 103, 5};
    Order o2 {2, Side::Sell, 109, 6};
    Order o3 {3, Side::Sell, 106, 3};

    book.add_order(o1);
    book.add_order(o2);
    book.add_order(o3);

    std::optional<int> ask = book.best_ask();
    REQUIRE(ask.has_value());
    CHECK(*ask == 103);
}

TEST_CASE("FIFO: first order in at a level is first out") {
    PriceLevel p {};
    Order o1 {1, Side::Buy, 103, 4};
    Order o2 {2, Side::Buy, 103, 6};

    p.add(o1);
    p.add(o2);

    CHECK(p.pop_front_order() == o1);
}