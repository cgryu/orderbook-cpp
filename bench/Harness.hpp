#include <chrono>

// Times a callable, returns elapsed nanoseconds.
template <typename F>
long long time_ns(F&& fn) {
    auto t0 = std::chrono::steady_clock::now();
    fn();
    auto t1 = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
}

inline long long percentile(const std::vector<long long>& sorted, double p) {
    std::size_t n = sorted.size();
    std::size_t idx = (std::size_t)(p * n);
    if (idx >= n) idx = n - 1;
    return sorted[idx];
}