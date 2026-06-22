#include "Workload.hpp"
#include "OrderBook.hpp"
#include "Order.hpp"
#include "Harness.hpp"
#include <iostream>
#include <chrono>
#include <algorithm>

static inline void apply_op(OrderBook& book, const Op& op) {
    if (op.type == OpType::Add) {
        auto trades = book.add_order(Order{op.id, op.side, op.price, op.quantity});
    } else {
        book.cancel(op.id);
    }
}

int main() {
    const std::size_t N {1000000};
    const std::size_t WARMUP {N/10};
    const int seed {42};
    WorkloadGenerator gen(seed);
    auto ops = gen.generate(N);

    std::vector<long long> durations;
    durations.reserve(N - WARMUP); 

    double ops_per_sec {};
    {
        // throughput
        OrderBook book {};
        for (std::size_t i = 0; i < WARMUP; ++i) apply_op(book, ops[i]);
        long long ns = time_ns([&] {
            for (std::size_t i = WARMUP; i < N; ++i) apply_op(book, ops[i]);
        });
        ops_per_sec = N - WARMUP / (ns / 1e9);
    }

    {
        // latency pass (latency per op)
        OrderBook book {};
        for (std::size_t i = 0; i < WARMUP; ++i) apply_op(book, ops[i]);
        for (std::size_t i = WARMUP; i < N; ++i) {
            auto t0 = std::chrono::steady_clock::now();
            apply_op(book, ops[i]);
            auto t1 = std::chrono::steady_clock::now();
            long long ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t1-t0).count();
            durations.push_back(ns);
        }
    }

    // percentile calculations
    auto worst = std::max_element(durations.begin(), durations.end());
    std::size_t worst_idx = worst - durations.begin();   // position in the timed run
    long long   worst_ns  = *worst;
    std::cout << "worst op: latency=" << worst_ns
            << "ns at timed-index " << worst_idx << "\n";
    std::sort(durations.begin(), durations.end());
    long long p99 {percentile(durations, 0.99)};
    long long p95 {percentile(durations, 0.95)};
    long long p50 {percentile(durations, 0.50)};
    long long max_ns {durations.back()};

    std::cout << "ops="        << (N - WARMUP)
          << "  thrpt="    << ops_per_sec << " ops/sec"
          << "  p50="      << p50 << "ns"
          << "  p95="      << p95 << "ns"
          << "  p99="      << p99 << "ns"
          << "  max="      << max_ns << "ns"
          << "\n";
    
}