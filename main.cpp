#include "raylib.h"
#include "game.hpp"
#include "types.hpp"
#include "algorithms.hpp"

#include <iostream>
using namespace std;

int main() {
    InitWindow(SCREEN_WIDTH, SCREEEN_HEIGHT, "snake-game");
    SetTargetFPS(60); 

    Screen currentScreen = Screen::MENU;
    SnakeGame game(10, 10);
    QLearningAgent qLAgent;

    float moveInterval = 0.15f;
    float moveTimer = 0.0f;
    int episode = 1;
    int highScore = 0;
    
    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition(); 

        if (currentScreen == Screen::MENU) {
            if (CheckCollisionPointRec(mousePos, BTN_QLEARNING) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                game.reset();
                currentScreen = Screen::GAME;
            }
        }
        else if (currentScreen == Screen::GAME) {
            if (game.isOver()) {
                if (IsKeyPressed(KEY_R)) game.reset();
            }
            else {
                if (IsKeyPressed(KEY_UP)) moveInterval = max(0.001f, moveInterval - 0.01f);
                if (IsKeyPressed(KEY_DOWN)) moveInterval += 0.02f;

                moveTimer += GetFrameTime();
                if (moveTimer >= moveInterval) {
                    moveTimer = 0.0f;

                    int state = game.getState();

                    Action action = qLAgent.chooseAction(state, true);

                    StepResult result = game.step(action);

                    qLAgent.update(state, action, result.reward, result.nextState, result.done);

                    if (result.done) {
                        if (game.getScore() > highScore) highScore = game.getScore();

                        qLAgent.decayEpsilon();
                        episode++;
                        game.reset();
                    }
                }
            }
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);

            switch(currentScreen) {
                case Screen::MENU: 
                    DrawMenuScreen(mousePos);
                    break;
                
                case Screen::GAME:
                    game.render();

                    DrawText(TextFormat("Episode: %d", episode), 25, SCREEEN_HEIGHT - 60, 24, BLACK);
                    DrawText(TextFormat("Score: %d (Record: %d)", game.getScore(), highScore), 25, SCREEEN_HEIGHT - 30, 24, DARKGREEN);
                    DrawText(TextFormat("Epsilon: %.3f", qLAgent.getEpsilon()), 350, SCREEEN_HEIGHT - 60, 24, MAROON);
                    DrawText(TextFormat("Velocity: %.3fs [Up/Down Arrow]", moveInterval), 350, SCREEEN_HEIGHT - 30, 20, DARKGRAY);

                    if (game.isOver())  DrawText("GAME OVER! Press R to restart!", 300, 300, 32, RED);

                    break;
            }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}