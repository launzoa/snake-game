#pragma once
#include "types.hpp"
#include "raylib.h"
#include <vector>
#include <string>

// Constantes para o algoritmo de Q-learning tabular
const int NUM_STATES = 512; // Quantidade de estados possíveis, i.e., 2^9 
const int NUM_ACTIONS = 3; // Número de ações que ela pode tomar, i.e., frente, virar a direita ou virar a esquerda 
const int NUM_FEATURES = 7; // Número de features que ela pode ter 

/// @brief Classe para modelar um agente de Q-learning
class QLearningAgent {
private:
    float qTable[NUM_STATES][NUM_ACTIONS]; // A tabela do Q-learning tabular

    float learningRate; // Constante de aprendizado (eta)
    float discountFactor; // Fator de desconto (gamma)
    float epsilon; // Fator de aleatoriedade (epsilon)
    float epsilonDecay; // Decaimento da aleatoriedade, i.e., aumenta a aleatoriedade com o tempo
    float epsilonMin; // Constante máxima de decaimento para epsilon  

public:
    // Inicialização do construtor, inicializando todas os parâmetros do método 
    QLearningAgent(float lr = 0.1f, float gamma = 0.9f, float eps = 1.0f, float d = 0.995f, float minEps = 0.01f);

    // Funções de treinamento
    Action chooseAction(int state, bool training); // Escolhe uma ação (estocástica, se training=true)
    void update(int state, Action action, float reward, int nextState, bool done); // Atualiza a tabela do Q-learning 
    void decayEpsilon(); // Gera o decaimento do epsilon

    // Getters e setters
    float getEpsilon() const { return epsilon; }; // Getter do valor de epsilon
    void setEpsilon(float eps) { epsilon = eps; }; // Setter para o valor de epsilon

    // Ainda não implementadas
    // void save(const std::string& filename) const;
    // void load(const std::string& filename);
};


/// @brief Classe para modelar um o algoritmo de Q-learning por função de aproximação
class LinearQLearningAgent {
private:
    float weights[NUM_ACTIONS][NUM_FEATURES]; // Coeficientes da função de ativação
    float bias[NUM_ACTIONS]; // Viés da função de aproximação

    float learningRate; // Taxa de aprendizado
    float discountFactor; // Fator de desconto 
    float epsilon; // Taxa de aleatoriedade
    float epsilonDecay; // Fator de decaimento da aleatoriedade (crescimento)
    float epsilonMin; // Valor mínimo para a aleatoriedade 

    std::vector<float> predict(const std::vector<float>& state) const; // Função para predizer os Q valores a partir das features 
public:
    // Construtor da classe
    LinearQLearningAgent(float lr = 0.01f, float gamma = 0.9f, float eps = 1.0f, float decay = 0.995f, float minEps = 0.01f);
    
    Action chooseAction(const std::vector<float>& state, bool training = true); // Função para escolher a ação
    // Função para atualizar o valor da função 
    void update(const std::vector<float>& state, Action action, float reward, const std::vector<float>& nextState, bool done);  
    void decayEpsilon();
    float getEpsilon() const { return epsilon; };
};