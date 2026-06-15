#pragma once

#include <deque>
#include "Order.hpp"

class PriceLevel {
public:
    PriceLevel() = default;

    void add(const Order& o);

    Order pop_front_order();

    bool empty() const;

    void print() const;

    int total_quantity() const;

    void reduce_front_qty(int quant);

    const Order& get_front() const;

private:
    std::deque<Order> m_orders {};

};