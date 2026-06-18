#pragma once

#include "Side.hpp"
#include "Order.hpp"

struct Trade {
    OrderId incomingID {};
    OrderId restingID {};
    int price {};
    int quantity {};
    Side aggressor {};
};