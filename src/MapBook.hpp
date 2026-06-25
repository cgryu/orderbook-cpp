#pragma once
#include "LevelBook.hpp"
#include <map>
#include <functional>

class MapBook: public LevelBook {
public:
    PriceLevel* best(Side side) override;
    PriceLevel& get_or_create(Side side, int price) override;
    void erase(Side side, int price) override;

private:
    std::map<int, PriceLevel, std::greater<int>> bids_ {};
    std::map<int, PriceLevel> asks_ {};
};