#include "MapBook.hpp"

PriceLevel* MapBook::best(Side side) {
    if (side == Side::Buy) {
        if (bids_.empty()) return nullptr;
        return &bids_.begin()->second;
    } else {
        if (asks_.empty()) return nullptr;
        return &asks_.begin()->second;
    }
}

PriceLevel& MapBook::get_or_create(Side side, int price) {
    if (side == Side::Buy)
        return bids_.try_emplace(price, price).first->second;
    else
        return asks_.try_emplace(price, price).first->second;
}

void MapBook::erase(Side side, int price) {
    if (side == Side::Buy) bids_.erase(price);
    else asks_.erase(price);
}