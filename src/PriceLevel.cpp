#include "PriceLevel.hpp"
#include <iostream>
#include <cassert>

void PriceLevel::add(const Order& o) {
    m_orders.push_back(o);
}

Order PriceLevel::pop_front_order() {
    assert(!m_orders.empty());
    Order val = m_orders.front();
    m_orders.pop_front();
    return val;
}

bool PriceLevel::empty() const {
    return m_orders.empty();
}

void PriceLevel::print() const {
    for (const Order& o : m_orders) {
        std::string sideName = o.side == Side::Buy ? "BUY" : "SELL";
        std::cout << "ID: " << o.id << '\n' << "Side: " << sideName << '\n' << "Price: " << o.price << '\n' << "Quantity: " << o.quantity << '\n';
    }
}

int PriceLevel::total_quantity() const {
    int quantity = 0;
    for (auto it = m_orders.begin(); it != m_orders.end(); ++it) {
        quantity += it->quantity;
    }
    return quantity;
}

void PriceLevel::reduce_front_qty(int quant) {
    auto& front = m_orders.front();
    assert(quant > 0 && quant < front.quantity);
    front.quantity -= quant;
}

const Order& PriceLevel::get_front() const {
    return m_orders.front();
}