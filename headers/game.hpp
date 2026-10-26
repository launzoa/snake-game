#pragma once 
#include "types.hpp"
#include "raylib.h"

#include <deque>
#include <vector>
#include <iostream>
#include <fstream>

const int SCREEN_WIDTH = 1280;
const int SCREEEN_HEIGHT = 720;

const Rectangle BTN_QLEARNING = {54, 180, 384, 72};

const int TILE_WIDTH = 103;
const int TILE_HEIGHT = 65;

class SnakeGame {
private:
    int gridWidth;
    int gridHeight;

    std::deque<Vector2> snake;
    Vector2 food;
    Direction currentDirection;
    int score;
    bool isGameOver;

    void placeFood();
    bool isCollision(Vector2 point) const;
    Direction getNewDirection(Action action) const;
    Vector2 getNeighbor(Direction direction) const;

public:
    SnakeGame(int width = 10, int height = 10);

    int reset();
    StepResult step(Action action);
    int getState() const;

    void render() const;

    bool isOver() const { return isGameOver; };
    int getScore() const { return score; };
};

//
void DrawMenuScreen(Vector2 mousePos);
void DrawGameScreen(Vector2 mousePos, std::vector<std::string>& grid);

std::vector<std::string> loadGrid(const std::string& filePath);
