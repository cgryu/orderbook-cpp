#include "OrderBook.hpp"
#include <iomanip>
#include <iostream>

void OrderBook::add_order(const Order& o) {
    if (o.side == Side::Buy) {
        auto [it, _] = bids_.try_emplace(o.price);
        it->second.add(o);
    }
    else {
        auto [it, _] = asks_.try_emplace(o.price);
        it->second.add(o);
    }
}

// this needs to find the highest ! which means use bids_ REMEMBER TO DEFERENCE THESE IF CALLED
std::optional<int> OrderBook::best_bid() const {
    if (bids_.empty()) {return std::nullopt;}
    else {
        auto it = bids_.begin();
        return std::optional{it->first};
    }
}

std::optional<int> OrderBook::best_ask() const {
    if (asks_.empty()) {return std::nullopt;}
    else {
        auto it = asks_.begin();
        return std::optional{it->first};
    }
}

void OrderBook::print_book() const {
    std::cout << "--- ORDER BOOK ---\n";
    std::cout << "ASKS\n";
    for (auto it = asks_.rbegin(); it != asks_.rend(); ++it) {
        int price = it->first;
        int quantity {it->second.total_quantity()};
        
        std::cout << std::setw(4) << price << " | " << quantity << '\n';
    }

    std::cout << "------------------\n";
    std::cout << "BIDS\n";
    for (auto it = bids_.begin(); it != bids_.end(); ++it) {
        int price = it->first;
        int quantity {it->second.total_quantity()};

        std::cout << std::setw(4) << price << " | " << quantity << '\n';
    }
}
