#include "algorithms.hpp"
using namespace std;


/// @brief Construtor da classe do agente de Q-learning
/// @param lr Taxa de aprendizado 
/// @param gamma Fator de desconto da recompensa futura
/// @param eps Taxa de aleatoriedade na escolha de uma ação (começa grande e vai tendendo à 1->minEps)
/// @param decay decaimento da taxa de aleatoriedade (conforme decaí, mais chance de aleatoriedade) 
/// @param minEps Limite máximo de decaimento da taxa de aleatoriedade
QLearningAgent::QLearningAgent(float lr, float gamma, float eps, float decay, float minEps)
    : learningRate(lr), discountFactor(gamma), epsilon(eps), epsilonDecay(decay), epsilonMin(minEps) {
    // Inicializa a matriz (tabela) do Q-learning 
    for (int s = 0; s < NUM_STATES; s++) {
        for (int a = 0; a < NUM_ACTIONS; a++) qTable[s][a] = 0.0f; // Com 0 em cada posição
    }
}

/// @brief Determina a ação do agente
/// @param state O estado atual do jogo
/// @param training Uma flag para determinar as escolhas do agente:
///     training = true: Há aleatoriedade na escolha das ações do agente (exploração)
///     training = false: A escolha das ações é baseada na otimalidade, i.e., no max Q*(s',a')
/// @return Retorna a ação tomada pelo agente no estado s
Action QLearningAgent::chooseAction(int state, bool training) {
    // Joga um dado aleatório (ótimo < epsilon, aleatório > epsilon)
    float randomValue = (float)GetRandomValue(0, 1000) / 1000.0f; 
    // Se estiver na fase de treinamento e o dado lançado aleatoriamente é menor que epsilon 
    if (training && randomValue < epsilon) { 
        int randomAction = GetRandomValue(0, NUM_ACTIONS-1); // Então toma uma ação aleatória
        return static_cast<Action>(randomAction); // E a retorna como escolhida para aquele estado
    }
    // Se a escolha não foi aleatória, então precisa ser ótima
    int bestAction = 0; // Variável para armazenar a melhor ação para aquele estado, começa pela primeira 
    float maxQ = qTable[state][0]; // Pega o Q-value para a primeira ação do estado atual
    for (int a = 1; a < NUM_ACTIONS; a++) { // Percorre por todas as possíveis ações (as 2 ações restantes)
        if (qTable[state][a] > maxQ) { // Se essa ação for melhor que a melhor atual
            maxQ = qTable[state][a]; // Atualiza ela como a melhor ação atual
            bestAction = a; // E atualiza o índice de melhor ação
        }
    }

    return static_cast<Action>(bestAction); // Retorna a melhor ação (sofre um cast de int->action) 
}


/// @brief Atualiza a tabela do Q-learning
/// @param state Estado do jogo atual
/// @param action Ação do jogo tomada
/// @param reward Recompensa para o estado
/// @param nextState Próximo estado
/// @param done Flag para encontrar um estado terminal
void QLearningAgent::update(int state, Action action, float reward, int nextState, bool done) {
    int actionIdx = static_cast<int>(action); // pega o índice daquela ação (i.e., o seu valor inteiro)

    float maxNextQ = 0.0f; // Variável para encontra o maior valor possível da tabela do Q-learning 
    if (!done) { // Se não for um estado terminal, então procura o maior Q-value para o estado atual
        maxNextQ = qTable[nextState][0]; // Inicializa a busca do maior Q-value começando pela primeira ação
        
        for (int a = 1; a < NUM_ACTIONS; a++) { // Percorre por todas as ações 
            if (qTable[nextState][a] > maxNextQ) maxNextQ = qTable[nextState][a]; // Atualizando sempre que encontrar uma maior
        }
    }

    float tdTarget = reward + discountFactor * maxNextQ; // Calcula o target, i.e., r + gamma * max Q(s,a)
    float tdError = tdTarget - qTable[state][actionIdx]; // Calcula o error, i.e., target - Q(s,a) 
    // Então, calcula o novo valor para aquele Q-value, i.e., Q(s,a) + eta * error
    qTable[state][actionIdx] = qTable[state][actionIdx] + learningRate * tdError; // Atualiza aquele Q(s,a)
}


/// @brief Atualiza o decaimento de epsilon, i.e., a taxa de aleatoriedade 
void QLearningAgent::decayEpsilon() {
    if (epsilon > epsilonMin) { // Se for maior que o valor mínimo de aleatoriedade
        epsilon *= epsilonDecay; // Decaí o fator de aleatoriedade 

        // Se após decair, se tornar menor que o mínimo 
        if (epsilon < epsilonMin) epsilon = epsilonMin; // Define ele como teto
    }
}


/// @brief Construtor da classe de Q-Learning com função de aproximação
/// @param lr Learning rate
/// @param gamma Fator de desconto
/// @param eps // Epsilon para aleatoriedade
/// @param decay // Taxa de decaimento
/// @param minEps  // Mínimo de valor para epsilon
LinearQLearningAgent::LinearQLearningAgent(float lr, float gamma, float eps, float decay, float minEps)
    : learningRate(lr), discountFactor(gamma), epsilon(eps), epsilonDecay(decay), epsilonMin(minEps) {
    for (int a = 0; a < NUM_ACTIONS; a++) { // Para cada ação
        bias[a] = 0.0f;// Define o bias como 0
        for (int f = 0; f < NUM_FEATURES; f++) { // E para cada feature
            weights[a][f] = 0.0f; // Define o peso como 0 
        }
    }
}

/// @brief Escolhe a ação tomada pelo agente
/// @param state O estado atual
/// @param training Flag para treinar (chance de aleatoriedade) ou não (otimalidade)
/// @return Retorna a ação tomada (escolhida)
Action LinearQLearningAgent::chooseAction(const vector<float>& state, bool training) {
    float randValue = GetRandomValue(0, 1000) / 1000.0f; // Sorteia um valor aleatoriamente

    if (training && randValue < epsilon) { // Se for treinamento e o valor sorteado for para aleatoriedade da ação
        int randomAction = GetRandomValue(0, NUM_ACTIONS - 1); // Escolhe uma índice de ação aleatoria
        return static_cast<Action>(randomAction); // Faz o cast explícito para a ação
    }

    vector<float> qValues = predict(state); // Tabela de Q-values para as features

    int bestAction = 0; // Variável para o índice de melhor ação (começa no 0) 
    float maxQ = qValues[0]; // Variável para maior Q-value da ação (começa na ação 0)
    for (int a = 1; a < NUM_ACTIONS; a++) { // Para cada ação
        if (qValues[a] > maxQ) { // Encontra o maior Q-value entre as ações 
            maxQ = qValues[a]; // Atualiza o seu Q-value
            bestAction = a; // Atualiza a ação tomada
        }
    }

    return static_cast<Action>(bestAction); // Retorna uma ação 
}

/// @brief Prediz os Q-values para as features e ações
/// @param state O estado atual
/// @return Retorna um vetor de Q-values 
vector<float> LinearQLearningAgent::predict(const vector<float>& state) const {
    vector<float> qValues(NUM_ACTIONS, 0.0f); // Tabela dos Q-valores por função de aproximação

    for (int a = 0; a < NUM_ACTIONS; a++) { // Para cada ação
        float q = bias[a]; // Inicializa o Q-value com o viés daquela ação 
        
        for (int f = 0; f < NUM_FEATURES; f++) {  // Para cada estado da ação (a)
            q += weights[a][f] * state[f]; // Faz o somatório das features 
        }

        qValues[a] = q; // Define o Q-value daquela ação na tabela 
    }

    return qValues; // Retorna o vetor de Q-values para as features
}



/// @brief Atualiza a tabela de Q-values para as features (função de aproximação)
/// @param state O estado atual
/// @param action A ação toamda 
/// @param reward A recompensa do estado-ação
/// @param nextState O próximo estado
/// @param done Flag caso seja um estado terminal
void LinearQLearningAgent::update(const vector<float>& state, Action action, float reward, const vector<float>& nextState, bool done) {
    int actIdx = static_cast<int>(action); // Pega o índice da ação tomada (cast)

    float maxNextQ = 0.0f; // Variável para encontrar o próximo melhor Q-value
    if (!done) { // Se não for um estado terminal
        vector<float> nextQValues = predict(nextState); // Procura o Q-value predito para o próximo estado
        maxNextQ = nextQValues[0]; // Define a primeira próxima ação como a melhor 
        
        for (int a = 1; a < NUM_ACTIONS; a++) { // E percorre por todas as ações
            if (nextQValues[a] > maxNextQ) maxNextQ = nextQValues[a]; // Comparando e atualizando, caso haja uma melhor
        }
    }

    float target = reward + discountFactor * maxNextQ; // Calcula o alvo, i.e., r + gamma * max_a' Q(s',a')
    float currentQ = predict(state)[actIdx]; // Prediz o Q-value para o estado atual e a ação tomada
    float tdError = target - currentQ; // Calcula o erro 

    for (int f = 0; f < NUM_FEATURES; f++) { // Então para cada feature
        weights[actIdx][f] += learningRate * tdError * state[f]; // Atualiza os valores das features
    }

    bias[actIdx] += learningRate * tdError; // Atualiza os valores dos bias
}

/// @brief Faz o decaimento do epsilon (para aumentar a aleatoriedade)
void LinearQLearningAgent::decayEpsilon() {
    if (epsilon > epsilonMin) { // Se o epsilon for maior que o mínimo
        epsilon *= epsilonDecay; // Atualiza ele
        if (epsilon < epsilonMin) epsilon = epsilonMin; // E define o mínimo como teto
    }
}
