#include "OrderBook.hpp"

#include "MapBook.hpp"
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <memory>

OrderBook::OrderBook() : levels_(std::make_unique<MapBook>()) {}

OrderBook::~OrderBook() = default;

std::vector<Trade> OrderBook::add_order(Order o) {
    std::vector<Trade> localTrades {};
    if (m_index.count(o.id) > 0) return localTrades;
    execute_trade(o, localTrades);
    if (o.quantity > 0) {
        PriceLevel& level = levels_->get_or_create(o.side, o.price);
        auto loc = level.add(o);
        OrderLocation oL = {o.side, o.price, loc};
        m_index.emplace(o.id, oL);
    }
    return localTrades;
}

std::optional<int> OrderBook::best_bid() const {
    PriceLevel* level = levels_->best(Side::Buy);
    if (!level) return std::nullopt;
    return level->price();
}

std::optional<int> OrderBook::best_ask() const {
    PriceLevel* level = levels_->best(Side::Sell);
    if (!level) return std::nullopt;
    return level->price();
}

bool OrderBook::crosses(const Order& incoming) const {
    if (incoming.side == Side::Buy) {
        auto ask = best_ask();
        return ask.has_value() && incoming.price >= *ask;
    }
    else {
        auto bid = best_bid();
        return bid.has_value() && incoming.price <= *bid;
    }
}

bool OrderBook::cancel(OrderId id) {
    auto it = m_index.find(id);
    if (it == m_index.end()) {
        return false;          
    }

    const OrderLocation& loc = it->second;
    PriceLevel& level = levels_->get_or_create(loc.side, loc.price);

    level.erase(loc.handle);

    if (level.empty()) {
        levels_->erase(loc.side, loc.price);
    }

    m_index.erase(id);
    return true;
    
}

std::optional<std::vector<Trade>> OrderBook::modify(OrderId id, int new_price, int new_qty) {
    if (new_price < 0 || new_qty <= 0) return std::nullopt;
    
    auto it = m_index.find(id);
    if (it == m_index.end()) return std::nullopt;

    const OrderLocation& loc = it->second;

    int old_qty = loc.handle->quantity;
    int old_price = loc.handle->price;
    Side side = loc.handle->side;

    if (new_qty > old_qty || old_price != new_price) {
        cancel(id);
        Order new_order {id, side, new_price, new_qty};
        return add_order(new_order);
    }
    else if (new_qty <= old_qty) {
        loc.handle->quantity = new_qty;
        return std::vector<Trade>{};
    }

    return std::nullopt;
}

void OrderBook::match_against(Side resting_side, Order& incoming, std::vector<Trade>& localTrades) {
        while (incoming.quantity > 0 && crosses(incoming)) {
            PriceLevel* level = levels_->best(resting_side);

            const Order& resting = level->get_front();
            int front_qty = resting.quantity;
            int fill = std::min(incoming.quantity, front_qty);
            incoming.quantity -= fill;

            Trade trade {incoming.id, resting.id, resting.price, fill, incoming.side};
            localTrades.push_back(trade);
            add_trade(trade);

            if (fill == front_qty) {
                m_index.erase(resting.id);
                level->pop_front_order();
                if (level->empty())
                    levels_->erase(resting_side, level->price());
            } else {
                level->reduce_front_qty(fill);
            }
        }
    }

void OrderBook::execute_trade(Order& incoming, std::vector<Trade>& localTrades) {
    if (incoming.side == Side::Buy)
        match_against(Side::Sell, incoming, localTrades);
    else
        match_against(Side::Buy, incoming, localTrades);
}

void OrderBook::add_trade(Trade trade) {
    trades_.push_back(trade);
}
