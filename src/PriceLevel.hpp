#pragma once

#include <deque>
#include "Order.hpp"
#include <list>
using OrderIt = std::list<Order>::iterator;

class PriceLevel {
public:
    PriceLevel() = default;

    OrderIt add(const Order& o);

    Order pop_front_order();

    bool empty() const;

    void print() const;

    int total_quantity() const;

    void reduce_front_qty(int quant);

    const Order& get_front() const;

    void erase(OrderIt it);

private:
    std::list<Order> m_orders {};

};