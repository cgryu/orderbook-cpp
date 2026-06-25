#pragma once

#include <map>
#include <optional>
#include <functional>
#include <vector>
#include <unordered_map>
#include <memory>
#include "Trade.hpp"
#include "PriceLevel.hpp"
#include "OrderLocation.hpp"

class LevelBook;

class OrderBook {
public: 
    OrderBook();

    ~OrderBook();

    std::vector<Trade> add_order(Order o);

    std::optional<int> best_bid() const;

    std::optional<int> best_ask() const;

    bool crosses(const Order& incoming) const;

    bool cancel(OrderId id);

    std::optional<std::vector<Trade>> modify(OrderId id, int new_price, int new_qty);

private:
    std::unique_ptr<LevelBook> levels_ {}; 
    std::vector<Trade> trades_ {};
    std::unordered_map<OrderId, OrderLocation> m_index {};

    void add_trade(Trade trade);

    void execute_trade(Order& incoming, std::vector<Trade>& localTrades);

    void match_against(Side resting_side, Order& incoming, std::vector<Trade>& localTrades);
};