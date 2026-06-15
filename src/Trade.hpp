#pragma once

#include "Side.hpp"
struct Trade {
    unsigned int incomingID {};
    unsigned int restingID {};
    int price {};
    int quantity {};
    Side aggressor {};
};