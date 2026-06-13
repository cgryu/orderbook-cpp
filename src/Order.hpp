#pragma once

#include "Side.hpp"
struct Order {
    unsigned int id {};
    Side side {};
    int price {};
    int quantity {};
};