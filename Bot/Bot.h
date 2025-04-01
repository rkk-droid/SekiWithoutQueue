#pragma once

#include <tuple>
#include <unordered_set>
#include <chrono>
#include <thread>
#include <tuple>

#include "../Board.h"
#include "../Config.h"

class Bot {
public:
    // Constructor
    Bot(Board& b, int d);

    // Methods
    void Play(bool& running);
    std::tuple<int, int> MakeDecision();

protected:
    Board& board_;
    int difficulty_;
};