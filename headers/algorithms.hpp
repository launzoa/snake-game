#pragma once
#include "types.hpp"
#include "raylib.h"
#include <vector>
#include <string>

const int NUM_STATES = 512;
const int NUM_ACTIONS = 3; 

class QLearningAgent {
private:
    float qTable[NUM_STATES][NUM_ACTIONS];

    float learningRate; 
    float discountFactor; 
    float epsilon;
    float epsilonDecay;
    float epsilonMin;

public:
    QLearningAgent(float lr = 0.1f, float gamma = 0.9f, float eps = 1.0f, float decay = 0.995f, float minEps = 0.01f);

    Action chooseAction(int state, bool training);
    void update(int state, Action action, float reward, int nextState, bool done);
    void decayEpsilon();

    float getEpsilon() const { return epsilon; };
    void setEpsilon(float eps) { epsilon = eps; };

    // void save(const std::string& filename) const;
    // void load(const std::string& filename);
};