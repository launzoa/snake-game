#pragma once 
#include "types.hpp"
#include "raylib.h"

#include <deque>
#include <vector>
#include <iostream>
#include <fstream>

// Constantes gerais para as dimensões da tela
const int SCREEN_WIDTH = 1280;
const int SCREEEN_HEIGHT = 720;

// Constantes para as dimensões de botões
const Rectangle BTN_QLEARNING = {54, 180, 384, 72};
const Rectangle BTN_QLEARNINGFUNC = {54, 270, 384, 72};

// Constantes para a grade do jogo
const int TILE_WIDTH = 103;
const int TILE_HEIGHT = 65;

/// @brief Classe para representar o jogo da cobrinha
class SnakeGame {
private:
    int gridWidth; // Dimensão horizontal da grade  
    int gridHeight; // Dimensão vertical da grade

    std::deque<Vector2> snake; // Mantém uma lista (deque para ter acesso ao primeiro elemento da lista) da posição da cobra
    Vector2 food; // A coordenada onde se encontra a comida
    Direction currentDirection; // A atual direção da cobra, i.e., o sentido (subindo, descendo, esquerda ou direita) 
    int score; // A pontuação atual do episódio
    bool isGameOver; // Se o jogo acabar (a cobra ter se empanturrado de comida)

    // Funções das classes 
    void placeFood(); // Setar o local da comida (aleatoriamente)
    bool isCollision(Vector2 point) const; // Verificar se houve colisão (melhor discutido na implementação da função)
    Direction getNewDirection(Action action) const; // Pegar a nova direção da cobra com base na ação 
    Vector2 getNeighbor(Direction direction) const; // 

public:
    SnakeGame(int width = 10, int height = 10); // Inicialização do construtor da classe, i.e., uma grade 10 x 10

    int reset(); // Resetar as configurações do jogo
    StepResult step(Action action); // Simular um step do jogo
    int getState() const; // Pegar o estado 
    void render() const; // Renderizar graficamente o jogo
    bool isOver() const { return isGameOver; }; // Getter para retornar a flag de fim de jogo 
    int getScore() const { return score; }; // Getter para retonrar o score do jogo
    std::vector<float> getFeatures() const; // Pega as features para a função por aproximação 
};

// Funções de desenho/interface da tela
void DrawMenuScreen(Vector2 mousePos); // Desenha a tela de menu
