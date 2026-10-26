#include "algorithms.hpp"
using namespace std;

QLearningAgent::QLearningAgent(float lr, float gamma, float eps, float decay, float minEps)
    : learningRate(lr), discountFactor(gamma), epsilon(eps), epsilonDecay(decay), epsilonMin(minEps) {
    for (int s = 0; s < NUM_STATES; s++) {
        for (int a = 0; a < NUM_ACTIONS; a++) qTable[s][a] = 0.0f;
    }
}


Action QLearningAgent::chooseAction(int state, bool training) {
    float randValue = (float)GetRandomValue(0, 1000) / 1000.0f;

    if (training && randValue < epsilon) {
        int randomAction = GetRandomValue(0, NUM_ACTIONS-1);
        return static_cast<Action>(randomAction);
    }

    int bestAction = 0;
    float maxQ = qTable[state][0];

    for (int a = 1; a < NUM_ACTIONS; a++) {
        if (qTable[state][a] > maxQ) {
            maxQ = qTable[state][a];
            bestAction = a;
        }
    }

    return static_cast<Action>(bestAction);
}


void QLearningAgent::update(int state, Action action, float reward, int nextState, bool done) {
    int actionIdx = static_cast<int>(action);

    float maxNextQ = 0.0f;
    if (!done) {
        maxNextQ = qTable[nextState][0];

        for (int a = 1; a < NUM_ACTIONS; a++) {
            if (qTable[nextState][a] > maxNextQ) maxNextQ = qTable[nextState][a];
        }
    }

    float tdTarget = reward + discountFactor * maxNextQ;
    float tdError = tdTarget - qTable[state][actionIdx];

    qTable[state][actionIdx] = qTable[state][actionIdx] + learningRate * tdError;
}


void QLearningAgent::decayEpsilon() {
    if (epsilon > epsilonMin) {
        epsilon *= epsilonDecay;
        if (epsilon < epsilonMin) epsilon = epsilonMin;
    }
}