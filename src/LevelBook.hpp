#pragma once
#include "Order.hpp"
#include "PriceLevel.hpp"

class LevelBook {
public:
    virtual ~LevelBook() = default;

    virtual PriceLevel* best(Side side) = 0;

    virtual PriceLevel& get_or_create(Side side, int price) = 0;

    virtual void erase(Side side, int price) = 0;

};