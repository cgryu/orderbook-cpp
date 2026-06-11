Section 1 - Definition
Limit order books are data structures used in market exchanges between buyers and sellers. Buyers will submit potential
bids, while sellers will submit potential asks. Each order should specify a price and quantity. 
The goal of the book is to organize all submitted bids and asks, and match any incoming orders 
that cross the spread and execute a trade. This specific implementation will use price-time
priority, meaning that the orders will be prioritized for the best price, then for whoever submitted earlier.

Section 2 - Features for Version 1
- Add an order (buy or sell)
- Cancel an existing order by ID
- Change an order (price/quantity)
- Match orders that cross the spread
- Execute market orders (buy/sell immediately at best available price)
- Accept limit orders (buy/sell whenever price crosses a given threshold, rest in book otherwise)

Section 3 - Non-Features for Version 1
- Networking (will be implementing in-process library ONLY)
- Database (no persistence)
- GUI
- Multiple instruments (one order book)
- Concurrency/Thread-safety (thread-safety will be implemented in the future)

Section 4 - Benchmarking
A major part of this project is testing the latency of different types of data structures, seeking out efficient ones,
and understanding why they are optimal. This will be done by measuring the following things:
- Throughput (orders/second)
- Latency distribution (with percentiles/addressing of tails)
- Data-Structure Comparison (std::maps vs sorted vectors vs flat array)