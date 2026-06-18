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
    Order b {2, Side::Buy, 101, quant2};

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

TEST_CASE("crosses() determines and compares best_ask correctly") {
    OrderBook book {};
    Order o1 {1, Side::Sell, 100, 5};
    Order o2 {2, Side::Buy, 100, 5};
    Order o3 {3, Side::Buy, 101, 5};
    Order o4 {4, Side::Buy, 99, 5};

    book.add_order(o1);

    CHECK(book.crosses(o2) == true);
    CHECK(book.crosses(o3) == true);
    CHECK(book.crosses(o4) == false);
}

TEST_CASE("crosses() determines and compares best_bid correctly") {
    OrderBook book {};
    Order o1 {1, Side::Buy, 101, 5};
    Order o2 {2, Side::Sell, 100, 5};
    Order o3 {3, Side::Sell, 101, 5};
    Order o4 {4, Side::Sell, 105, 5};

    book.add_order(o1);

    CHECK(book.crosses(o2) == true);
    CHECK(book.crosses(o3) == true);
    CHECK(book.crosses(o4) == false);
}

TEST_CASE("crosses() works correctly for empty opposite side"){
    OrderBook book {};

    Order o1 {1, Side::Buy, 101, 1};
    Order o2 {2, Side::Sell, 102, 2};

    CHECK(book.crosses(o1) == false);
    CHECK(book.crosses(o2) == false);
}

TEST_CASE("reduce_front_qty reduces quantity of front order without removing") {
    PriceLevel p {};
    Order o {1, Side::Buy, 100, 100};
    p.add(o);
    p.reduce_front_qty(99);

    CHECK(p.get_front().quantity == 1);
}

// incoming smaller than front, so reduce quant of front but dont pop or reorder anything
TEST_CASE("Partial Resting") {
    OrderBook book {};
    Order o1 {1, Side::Buy, 100, 100};
    Order o2 {2, Side::Buy, 100, 100};
    Order o3 {3, Side::Sell, 99, 99};

    book.add_order(o1);
    book.add_order(o2);
    auto trades = book.add_order(o3);

    CHECK(trades.size() == 1);
    CHECK(trades[0].price == 100);
    CHECK(trades[0].quantity == 99);
    CHECK(trades[0].incomingID == 3);
    CHECK(trades[0].restingID == 1);

    auto bid = book.best_bid();
    REQUIRE(bid.has_value());
    CHECK(*bid == 100);
}

// fill one level fully, re-check next level, rest the leftover bc doesn't cross anymore
TEST_CASE("FILL BUT STOP ON NO CROSS") {
    OrderBook book {};
    Order o1 {1, Side::Buy, 100, 10};
    Order o2 {2, Side::Buy, 95, 10};
    Order o3 {3, Side::Sell, 100, 15};

    book.add_order(o1);
    book.add_order(o2);
    auto trades = book.add_order(o3);

    CHECK(trades.size() == 1);
    CHECK(trades[0].price == 100);
    CHECK(trades[0].quantity == 10);
    CHECK(trades[0].incomingID == 3);
    CHECK(trades[0].restingID == 1);

    auto bid = book.best_bid();
    REQUIRE(bid.has_value());
    CHECK(*bid == 95);

    auto ask = book.best_ask();
    REQUIRE(ask.has_value());
    CHECK(*ask == 100);
}

// make sure trade price is the resting order's price, and that becomes the level / map key, not the incoming limit price
TEST_CASE("MAKER PRICE") {
    OrderBook book {};
    Order o1 {1, Side::Buy, 100, 100};
    Order o2 {2, Side::Sell, 95, 101};

    book.add_order(o1);
    auto trades = book.add_order(o2);

    CHECK(trades.size() == 1);
    CHECK(trades[0].price == 100);
    CHECK(trades[0].quantity == 100);
    CHECK(trades[0].incomingID == 2);
    CHECK(trades[0].restingID == 1);

    auto ask = book.best_ask();
    REQUIRE(ask.has_value());
    CHECK(*ask == 95);
}

// if matched to two different trades, it correctly outputs as two trades
TEST_CASE("SAME-PRICE TRADE") {
    OrderBook book {};
    Order o1 {1, Side::Buy, 100, 7};
    Order o2 {2, Side::Buy, 100, 3};
    Order o3 {3, Side::Sell, 100, 9};

    book.add_order(o1);
    book.add_order(o2);
    auto trades = book.add_order(o3);

    CHECK(trades.size() == 2);
    CHECK(trades[0].price == 100);
    CHECK(trades[0].quantity == 7);
    CHECK(trades[0].incomingID == 3);
    CHECK(trades[0].restingID == 1);

    CHECK(trades[1].price == 100);
    CHECK(trades[1].quantity == 2);
    CHECK(trades[1].incomingID == 3);
    CHECK(trades[1].restingID == 2);

    auto bid = book.best_bid();
    REQUIRE(bid.has_value());
    CHECK(*bid == 100);
}

TEST_CASE("CANCEL ORDER REMOVES IT FROM THE LIST") {
    OrderBook book {};
    Order o1 {1, Side::Buy, 100, 3};
    
    book.add_order(o1);

    bool status = book.cancel(1);

    REQUIRE(status == true);
    CHECK(book.best_bid() == std::nullopt);
}

TEST_CASE("CANCELING MIDDLE ORDER DOES NOT IMPACT OTHERS") {
    OrderBook book {};
    Order o1 {1, Side::Buy, 100, 3};
    Order o2 {2, Side::Buy, 100, 4};
    Order o3 {3, Side::Buy, 100, 3};

    book.add_order(o1);
    book.add_order(o2);
    book.add_order(o3);

    bool status = book.cancel(2);

    REQUIRE(status == true);
    CHECK(book.best_bid() == 100);
}

TEST_CASE("CANCELING AN INVALID ID DOESN'T WORK") {
    OrderBook book {};
    Order o1 {1, Side::Sell, 100, 3};

    book.add_order(o1);

    CHECK(book.cancel(o1.id) == true);
    CHECK(book.cancel(999) == false);

    CHECK(book.best_bid() == std::nullopt);
}

TEST_CASE("FILLED ORDER IS UNCANCELABLE") {
    OrderBook book {};
    Order o1 {1, Side::Sell, 100, 3};
    Order o2 {2, Side::Buy, 100, 3};

    book.add_order(o1);
    auto trades = book.add_order(o2);

    CHECK(trades.size() == 1);
    CHECK(book.cancel(1) == false);
}

TEST_CASE("duplicate id is rejected") {
    OrderBook book {};

    book.add_order({1, Side::Buy, 100, 5});
    auto trades = book.add_order({1, Side::Buy, 100, 99}); 
    
    CHECK(trades.empty());               
    CHECK(book.cancel(1) == true);          
    CHECK(book.best_bid() == std::nullopt);
}

TEST_CASE("LOWER NEW_QTY DOESN'T CHANGE FIFO") {
    OrderBook book {};
    Order o1 {1, Side::Buy, 100, 5};
    Order o2 {2, Side::Buy, 100, 7};
    Order o3 {3, Side::Sell, 100, 12};

    book.add_order(o1);
    book.add_order(o2);

    book.modify(1, 100, 3);
    auto trades = book.add_order(o3);

    CHECK(trades.size() == 2);
    CHECK(trades[0].quantity == 3);
    CHECK(trades[1].quantity == 7);
    CHECK(book.best_ask() == 100);
}

TEST_CASE("LEVEL TOTAL-QUANTITY REFLECTS THE DECREASE") {
    OrderBook book {};
    Order o1 {1, Side::Buy, 100, 5};
    Order o2 {2, Side::Sell, 105, 5};

    book.add_order(o1);
    book.add_order(o2);

    auto result = book.modify(1, 105, 5);
    REQUIRE(result.has_value());
    CHECK(result->size() == 1);              
    CHECK((*result)[0].price == 105);
    CHECK((*result)[0].quantity == 5);
}

TEST_CASE("MODIFY PRICE INTO A CROSS TRADES AT MAKER PRICE") {
    OrderBook book {};
    Order o1 {1, Side::Buy, 100, 10};
    Order o2 {2, Side::Buy, 100, 25};
    Order o3 {3, Side::Sell, 100, 25};

    book.add_order(o1);
    book.add_order(o2);

    book.modify(1, 100, 15);
    auto trades = book.add_order(o3);

    REQUIRE(trades.size() == 1);
    CHECK(trades[0].quantity == 25);
}

TEST_CASE("MODIFY UNKNOWN ID RETURNS NULLOPT") {
    OrderBook book {};
    
    auto result = book.modify(1, 100, 5);
    CHECK(result == std::nullopt);
}