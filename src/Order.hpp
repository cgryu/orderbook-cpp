#pragma once

#include "Side.hpp"
struct Order {
    unsigned int id {};
    Side side {};
    int price {};
    int quantity {};

    bool operator==(const Order&) const = default;
};