#include "OrderBook.hpp"

int main() {
    OrderBook sample {};
    Order AAPL {1, Side::Sell, 100, 5};
    Order MSFT {2, Side::Sell, 103, 6};
    Order JPM {3, Side::Sell, 106, 4};
    Order AMZN {4, Side::Sell, 103, 5};
    Order SPCX {5, Side::Buy, 100, 10};
    Order ROKU {6, Side::Buy, 106, 6};
    Order TSM {7, Side::Buy, 106, 8};
    Order GOOGL {8, Side::Buy, 102, 9};
    sample.add_order(AAPL);
    sample.add_order(MSFT);
    sample.add_order(JPM);
    sample.add_order(AMZN);
    sample.add_order(SPCX);
    sample.add_order(ROKU);
    sample.add_order(TSM);
    sample.add_order(GOOGL);
    
    sample.print_book();
}