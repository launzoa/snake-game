#pragma once
#include <vector>

enum class Screen {
    MENU,
    GAME 
};

enum class Direction {
    UP = 0,
    DOWN = 1,
    LEFT= 2,
    RIGHT = 3
};

enum class Action {
    STRAIGHT = 0,
    TURN_LEFT = 1,
    TURN_RIGHT = 2
};

struct StepResult {
    int nextState;
    float reward;
    bool done;
    int score;
};