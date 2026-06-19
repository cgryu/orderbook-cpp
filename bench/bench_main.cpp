#include "Workload.hpp"
#include "OrderBook.hpp"
#include "Order.hpp"
#include <iostream>

int main() {
    WorkloadGenerator gen(42);
    auto ops = gen.generate(10000);

    OrderBook book;
    std::size_t adds = 0, cancels = 0, cancelHits = 0, addsThatTraded = 0;
    for (const Op& op : ops) {
        if (op.type == OpType::Add) {
            auto trades = book.add_order(Order{op.id, op.side, op.price, op.quantity});
            if (!trades.empty()) ++addsThatTraded;
            ++adds;
        } else {
            if (book.cancel(op.id)) ++cancelHits;
            ++cancels;
        }
    }
    std::cout << "ops="   << ops.size()
            << " adds=" << adds
            << " addsThatTraded = " << addsThatTraded
            << " cancels=" << cancels
            << " hitRate=" << (cancels ? double(cancelHits) / cancels : 0.0)
            << "\n";
}