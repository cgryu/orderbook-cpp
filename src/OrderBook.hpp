#pragma once

#include <map>
#include <optional>
#include <functional>
#include "PriceLevel.hpp"

class OrderBook {
public: 
    void add_order(const Order& o);

    std::optional<int> best_bid() const;

    std::optional<int> best_ask() const;

    void print_book() const;

private:
    std::map<int, PriceLevel, std::greater<int>> bids_;
    std::map<int, PriceLevel> asks_;
};