cmake_minimum_required(VERSION 3.16)
project(orderbook CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(orderbook
    src/main.cpp
    src/PriceLevel.cpp
    src/OrderBook.cpp
)
add_executable(tests 
tests/test_main.cpp 
src/PriceLevel.cpp 
src/OrderBook.cpp
)
target_include_directories(tests PRIVATE src third_party)
target_compile_options(orderbook PRIVATE -Wall -Wextra)
target_compile_options(tests     PRIVATE -Wall -Wextra)