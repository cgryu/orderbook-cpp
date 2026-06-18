#include "OrderBook.hpp"
#include <iomanip>
#include <iostream>
#include <algorithm>

std::vector<Trade> OrderBook::add_order(Order o) {
    std::vector<Trade> localTrades {};
    if (m_index.count(o.id) > 0) return localTrades;
    execute_trade(o, localTrades);
    if (o.quantity > 0) {
        PriceLevel* level = nullptr;
        if (o.side == Side::Buy) {
            level = &bids_.try_emplace(o.price).first->second;
        }
        else {
            level = &asks_.try_emplace(o.price).first->second;
        }
        auto loc = level->add(o);
        OrderLocation oL = {o.side, o.price, loc};
        m_index.emplace(o.id, oL);
    }
    return localTrades;
}

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

    PriceLevel* level = nullptr;
    const OrderLocation& loc = it->second;

    if (loc.side == Side::Buy) {
        level = &bids_.find(loc.price)->second;
    }
    else {
        level = &asks_.find(loc.price)->second;
    }

    level->erase(loc.handle);

    if (level->empty()) {
        if (loc.side == Side::Buy) bids_.erase(loc.price);
        else                       asks_.erase(loc.price);
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

void OrderBook::execute_trade(Order& incoming, std::vector<Trade>& localTrades) {
    if (incoming.side == Side::Buy)
        match_against(asks_, incoming, localTrades);
    else
        match_against(bids_, incoming, localTrades);
}

void OrderBook::add_trade(Trade trade) {
    trades_.push_back(trade);
}
