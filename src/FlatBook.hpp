#pragma once
#include "LevelBook.hpp"
#include "PriceLevel.hpp"
#include <vector>
#include <optional>
#include <limits>

class FlatBook: public LevelBook {
public:
    explicit FlatBook(size_t bound);
    PriceLevel* best(Side s) override;
    PriceLevel& get_or_create(Side side, int price) override;
    void erase(Side side, int price) override;
    void update_best_on_insert(Side s, size_t idx);

private:
    static constexpr size_t NO_BEST = std::numeric_limits<size_t>::max();
    std::vector<std::optional<PriceLevel>> bids_;
    std::vector<std::optional<PriceLevel>> asks_;

    size_t best_ask_tick {NO_BEST};
    size_t best_bid_tick {NO_BEST};
};