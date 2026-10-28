#include "raylib.h"
#include "game.hpp"
#include "types.hpp"
#include "algorithms.hpp"

#include <iostream>
using namespace std;

int main() {
    InitWindow(SCREEN_WIDTH, SCREEEN_HEIGHT, "snake-game"); // Inicializa a tela do jogo
    SetTargetFPS(60); // Seta os fps

    Screen currentScreen = Screen::MENU; // Define o menu como tela inicial
    SnakeGame game(10, 10); // Cria a grid do game
    QLearningAgent qLAgent; // Cria o agente do Q-learning tabular
    LinearQLearningAgent linearQLAgent; // Cria o agente do Q-learning por função de aproximação
    
    float moveInterval = 0.15f; // O intervalo com que a cobra deve atualizar sua movimentação
    float moveTimer = 0.0f; // Um cronômetro de tempo (contador) para atingir o intervalo de tempo
    int episode = 1; // Inicia os episódios (épocas) 
    int highScore = 0; // A pontuação máxima, i.e., quantas comidas comeu no episodio
    
    bool screenTabular = false;
    while (!WindowShouldClose()) { // Enquanto a tela não foi fechada
        Vector2 mousePos = GetMousePosition();  // Pega a posição do mouse

        if (currentScreen == Screen::MENU) { // Verifica se a tela é de menu
            // Se for, verifica se houve click no botão de Q-learning tabular
            if (CheckCollisionPointRec(mousePos, BTN_QLEARNING) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                game.reset(); // Então reseta/inicializa os parâmetros do jogo
                currentScreen = Screen::GAME; // E muda para a tela de jogo
                screenTabular = true;
            }
            //SVerifica houve click no botão de Q-learning por função 
            if (CheckCollisionPointRec(mousePos, BTN_QLEARNINGFUNC) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                game.reset(); // Então reseta/inicializa os parâmetros do jogo
                currentScreen = Screen::GAME; // E muda para a tela de jogo
                screenTabular = false;
            }
        }
        // Se a tela é do jogo
        else if (currentScreen == Screen::GAME) {
            if (game.isOver()) { // Verifica se o jogo acabou (venceu), podendo ser retado com R
                if (IsKeyPressed(KEY_R)) game.reset();
            }
            // Se ainda não acabou
            else {
                // Pode aumentar (up) ou diminuir (down) a velocidade de movimento
                if (IsKeyPressed(KEY_UP)) moveInterval = max(0.001f, moveInterval - 0.01f);
                if (IsKeyPressed(KEY_DOWN)) moveInterval += 0.02f; 
                
                moveTimer += GetFrameTime(); // Incrementa o contador de tempo por frame-tempo do jogo
                if (moveTimer >= moveInterval) { // Se ele atingir o intervalo de tempo do jogo
                    moveTimer = 0.0f; // Reseta o cronômetro de tempo
                    bool isTraining = (episode < 1000); // Flag para treinar
                    bool done = false; // Estado terminal

                    if (screenTabular) {
                        int state = game.getState(); // Pega o estado do jogo
                        
                        Action action = qLAgent.chooseAction(state, isTraining); // Toma uma ação (em treinamento ou ótima)
                        StepResult result = game.step(action); // Gera o próximo passo com base na ação
                        qLAgent.update(state, action, result.reward, result.nextState, result.done); // Atualiza a Q-tabela
                        done = result.done; // Atualiza a flag de estado terminal 
                    }
                    else {
                        vector<float> state = game.getFeatures(); // Pega as features do estado
                        Action action = linearQLAgent.chooseAction(state, isTraining); // Toma a ação (em treino ou ótimo) 
                        StepResult result = game.step(action); // Gera o próximo passo com base na ação
                        vector<float> nextState = game.getFeatures(); // Pega o próximo estado de features
                        linearQLAgent.update(state, action, result.reward, nextState, result.done); // Atualiza os Q-values do agente
                        done = result.done; // Atualiza a flag de estado terminal
                    }

                    if (done) { // Se for um estado terminal
                        // Caso o score do episódio seja o maior até agora
                        if (game.getScore() > highScore) highScore = game.getScore(); // Define ele como o novo máximo 

                        // Aumenta a aleatoridade (reduz o epislon) a depender do algoritmo de Q-learning
                        if (screenTabular) qLAgent.decayEpsilon(); 
                        else linearQLAgent.decayEpsilon();

                        episode++; // Incrementa o número de episódios 

                        game.reset(); // Reseta os parâmetros do jogo
                    }

                }
            }
        }

        BeginDrawing(); // Começa a desenhar
            ClearBackground(RAYWHITE); // Cor de fundo padrão do raylib 

            switch(currentScreen) { // Dada a tela atual

                case Screen::MENU:  // Se for a de menu
                    DrawMenuScreen(mousePos); // Desenha a tela de menu
                break;
                
                case Screen::GAME: // Se for a de jogo
                    game.render(); // Desenha a tela do jogo (renderiza)
                    
                    // Pega os Epsilon / Nomes dos algoritmos
                    float currentEps = screenTabular ? qLAgent.getEpsilon() : linearQLAgent.getEpsilon();
                    const char* algoName = screenTabular ? "Tabular Q-Learning" : "Linear Approx Q-Learning";
                   
                    // Desenha os textos gráficos 
                    DrawText(TextFormat("Algoritmo: %s", algoName), 25, 20, 22, DARKGRAY);
                    DrawText(TextFormat("Episode: %d", episode), 25, SCREEEN_HEIGHT - 60, 24, BLACK);
                    DrawText(TextFormat("Score: %d (Record: %d)", game.getScore(), highScore), 25, SCREEEN_HEIGHT - 30, 24, DARKGREEN);
                    DrawText(TextFormat("Epsilon: %.3f", currentEps), 350, SCREEEN_HEIGHT - 60, 24, MAROON);
                    DrawText(TextFormat("Velocity: %.3fs [Up/Down Arrow]", moveInterval), 350, SCREEEN_HEIGHT - 30, 20, DARKGRAY);
                    // Se o jogo tiver acabado 
                    if (game.isOver())  DrawText("GAME OVER! Press R to restart!", 300, 300, 32, RED);

                break;
            }

        EndDrawing(); // Fim do desenho
    }

    CloseWindow(); // Fecha a janela
    return 0;
}