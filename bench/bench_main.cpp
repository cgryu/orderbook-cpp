#include "Workload.hpp"
#include "OrderBook.hpp"
#include "Order.hpp"
#include "Harness.hpp"
#include <iostream>
#include <chrono>

static inline void apply_op(OrderBook& book, const Op& op) {
    if (op.type == OpType::Add) {
        auto trades = book.add_order(Order{op.id, op.side, op.price, op.quantity});
    } else {
        book.cancel(op.id);
    }
}

const char* to_string(OpType t) {
    switch(t) {
        case OpType::Add: return "Add";
        case OpType::Cancel: return "Cancel";
    }
    return "?";
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
        ops_per_sec = (N - WARMUP) / (ns / 1e9);
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

    // op struct vector construction
    struct opLatency {
        std::size_t index;
        long long latency;
        OpType opType;
    };

    std::vector<opLatency> worstOps; 
    for (std::size_t i = 0; i < durations.size(); ++i) {
        worstOps.push_back({i + WARMUP, durations[i], ops[i + WARMUP].type});
    }

    std::partial_sort(worstOps.begin(), worstOps.begin() + 20, worstOps.end(),
    [](const opLatency& a, const opLatency& b) {
        return a.latency > b.latency; 
    });

    // percentile calculations
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

    for (std::size_t i = 0; i < 20; i++) {
        std::cout 
        << "Rank: " << i << "       " << "Index: " << worstOps[i].index << "     " << "Latency: " << worstOps[i].latency << "        " << "OpType: " << to_string(worstOps[i].opType) << '\n';
    }
    
}