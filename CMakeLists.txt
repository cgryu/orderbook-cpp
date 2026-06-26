cmake_minimum_required(VERSION 3.16)
project(orderbook CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra)

# --- engine lib ---
add_library(orderbook_lib
    src/PriceLevel.cpp
    src/OrderBook.cpp
    src/MapBook.cpp
    src/FlatBook.cpp
)
target_include_directories(orderbook_lib PUBLIC src)

# --- demo ---
add_executable(orderbook src/main.cpp)
target_link_libraries(orderbook PRIVATE orderbook_lib)

# --- tests ---
add_executable(tests tests/test_main.cpp)
target_link_libraries(tests PRIVATE orderbook_lib)
target_include_directories(tests PRIVATE third_party)

# --- benchmark --- 
add_executable(bench bench/bench_main.cpp)
target_link_libraries(bench PRIVATE orderbook_lib)