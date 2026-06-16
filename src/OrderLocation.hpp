#pragma once

#include "Side.hpp"
#include "Order.hpp"
#include <list>
using OrderIt = std::list<Order>::iterator;

struct OrderLocation {
    Side side {};
    int price {};
    OrderIt handle {};
};