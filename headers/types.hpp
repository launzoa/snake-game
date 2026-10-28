#pragma once
#include <vector>

/// Estrutura para representar as diferentes telas do jogo
enum class Screen {
    MENU,
    GAME 
};

/// Estrutura para representar as 4 possíveis direções da cobra.
enum class Direction {
    UP = 0,
    RIGHT = 1,
    DOWN= 2,
    LEFT = 3
};

/// Estrutura para representar uma ação (a) tomada dado algum estado (s)
enum class Action {
    STRAIGHT = 0,
    TURN_LEFT = 1,
    TURN_RIGHT = 2
};

/// Estrutura para representar uma tupla de resultado 
struct StepResult {
    int nextState;
    float reward;
    bool done;
    int score;
};