#pragma once

#include "Side.hpp"
#include <cstdint>

using OrderId = std::uint64_t;
struct Order {
    OrderId id {};
    Side side {};
    int price {};
    int quantity {};

    bool operator==(const Order&) const = default;
};