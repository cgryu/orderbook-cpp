#pragma once

#include <map>
#include <optional>
#include <functional>
#include <vector>
#include "Trade.hpp"
#include "PriceLevel.hpp"

class OrderBook {
public: 
    std::vector<Trade> add_order(Order o);

    std::optional<int> best_bid() const;

    std::optional<int> best_ask() const;

    void print_book() const;

    bool crosses(const Order& incoming) const;

private:
    std::map<int, PriceLevel, std::greater<int>> bids_ {};
    std::map<int, PriceLevel> asks_ {};
    std::vector<Trade> trades_ {};

    void add_trade(Trade trade);

    void execute_trade(Order& incoming, std::vector<Trade>& localTrades);

    template <class Book>
    void match_against(Book& book, Order& incoming, std::vector<Trade>& localTrades) {
        while (incoming.quantity > 0 && crosses(incoming)) {
            auto it = book.begin();
            int front_qty = it->second.get_front().quantity;
            int fill = std::min(incoming.quantity, front_qty);
            incoming.quantity -= fill;

            const Order& resting = it->second.get_front();
            Trade trade {incoming.id, resting.id, resting.price, fill, incoming.side};
            localTrades.push_back(trade);
            add_trade(trade);

            if (fill == front_qty) {
                it->second.pop_front_order();
                if (it->second.empty()) book.erase(it);
            } else {
                it->second.reduce_front_qty(fill);
            }
        }
    }
};