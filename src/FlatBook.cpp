#include "FlatBook.hpp"
#include <iostream>

FlatBook::FlatBook(size_t bound) 
    : bids_{bound}
    , asks_{bound} 
    {}

PriceLevel* FlatBook::best(Side s) {
    size_t t = (s == Side::Buy) ? best_bid_tick : best_ask_tick;
    if (t == NO_BEST) return nullptr;
    auto& arr = (s == Side::Buy) ? bids_ : asks_;
    return &*arr[t];
}

PriceLevel& FlatBook::get_or_create(Side s, int price) {
    auto& arr = (s == Side::Buy) ? bids_ : asks_;
    size_t idx = static_cast<size_t>(price);

    if (!arr[idx]) {
        arr[idx].emplace(price);
        update_best_on_insert(s, idx);
    }
    return *arr[idx];
}

void FlatBook::update_best_on_insert(Side s, size_t idx) {
    if (s == Side::Buy) {
        if (best_bid_tick == NO_BEST || idx > best_bid_tick) best_bid_tick = idx;
    }
    else {
        if (best_ask_tick == NO_BEST || idx < best_ask_tick) best_ask_tick = idx;
    }
}

void FlatBook::erase(Side s, int price) {
    size_t idx = static_cast<size_t>(price);
    auto& arr = (s == Side::Buy) ? bids_ : asks_;

    arr[idx].reset();

    if (s == Side::Buy) {
        if (idx != best_bid_tick) return;

        size_t i = best_bid_tick;
        while (i-- > 0) {
            if (bids_[i]) {best_bid_tick = i; return; }
        }
        best_bid_tick = NO_BEST;
    }
    else {
        if (idx != best_ask_tick) return;
        
        for (size_t i = best_ask_tick; i < asks_.size(); ++i) {
            if (asks_[i]) {best_ask_tick = i; return; }
        }
        best_ask_tick = NO_BEST;
    }
}