#pragma once
#include "Order.hpp"
#include <vector>
#include <random>
#include <cstdint>

enum class OpType {Add, Cancel};

struct Op {
    OpType type;
    OrderId id;
    Side side;
    int price;
    int quantity;
};

class WorkloadGenerator {
public:
    explicit WorkloadGenerator(std::uint64_t seed) : rng_{seed} {}

    std::vector<Op> generate(std::size_t n) {
        std::vector<Op> ops;
        ops.reserve(n);

        std::uniform_int_distribution<int> sizeDist(1, 100);
        std::uniform_int_distribution<int> buyPriceDist(mid_ - band_, mid_ - 1);
        std::uniform_int_distribution<int> sellPriceDist(mid_ + 1, mid_ + band_);
        std::uniform_int_distribution<int> sideDist(0, 1);
        std::uniform_real_distribution<double> coin(0.0, 1.0);

        for (std::size_t i = 0; i < n; ++i) {
            if (coin(rng_) < cancelProb_ && !live_.empty()) {
                std::uniform_int_distribution<std::size_t> pick(0, live_.size() - 1);
                std::size_t idx = pick(rng_);
                OrderId id = live_[idx];

                ops.push_back({OpType::Cancel, id, Side::Buy, 0, 0});

                live_[idx] = live_.back();
                live_.pop_back();
                continue;
            }

            OrderId id = nextID_++;
            Side side = (sideDist(rng_) == 0) ? Side::Buy : Side::Sell;
            int price {};
            if (side == Side::Buy) {
                if (coin(rng_) < marketable_prob_) {
                    price = mid_ + band_;
                } else {
                    price = buyPriceDist(rng_);
                }
            } else {
                if (coin(rng_) < marketable_prob_) {
                    price = mid_ - band_;
                } else {
                    price = sellPriceDist(rng_);
                }
            }
            int qty = sizeDist(rng_);

            ops.push_back({OpType::Add, id, side, price, qty});
            live_.push_back(id);
        }
        return ops;
    }

private: 
    std::mt19937_64 rng_;
    OrderId nextID_ = 1;
    std::vector<OrderId> live_; 
    int mid_ = 1000;
    int band_ = 20;
    double cancelProb_ {0.3};
    double marketable_prob_{0.1};
};