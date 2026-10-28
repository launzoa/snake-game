#include "game.hpp"
using namespace std;

/// @brief Inicializa o construtor da classe do jogo
/// @param width Largura do grid (tipicamente 10)
/// @param height Altura do grid (tipicamente 10)
SnakeGame::SnakeGame(int width, int height) : gridWidth(width), gridHeight(height) {
    reset(); // Sempre ao inicializar o construtor, reseta todos os parâmetros do jogo
}


/// @brief Define o local do surgimento da comida 
void SnakeGame::placeFood() {
    while (true) { // Enquanto não ter uma interrupção do loop
        float posX = GetRandomValue(0, gridWidth-1); // Randomiza um x do grid (0 - 9) 
        float posY = GetRandomValue(0, gridHeight-1); // Randomiza um y do grid (0 - 9)
        Vector2 randomPoint = {posX, posY}; // Define essas posições como o ponto aleatório 

        bool onSnake = false; // Flag para verificar se o ponto sorteado não está dentro da cobra 
        for (const Vector2 part : snake) { // Para cada parte da cobra
            // Verifica se o ponto sorteado está contido nela 
            if (part.x == randomPoint.x && part.y == randomPoint.y) { 
                onSnake = true; // Se estiver, então ativa a flag de colisão com o corpo da cobra
                break; // E sai do loop for
            }
        }

        // Se a flag de colisão estiver ativa, então não é um ponto válido para se ter a comida 
        if (!onSnake) { // Caso contrário, i.e., a posição da comida esteja em um lugar vazio do grid
            food = randomPoint; // Define esse ponto aleatório como a localização da comida 
            break; //
        }
    }
}


/// @brief Verifica se houve colisão entre um ponto e as bordas do grid ou partes da cobra
///     A lógica do movimento da cobra é que a cada step, ela se movimenta em um ponto para alguma direção 
///     Logo, para verificar se houve alguma colisão, passamos o seu ponto de movimentação e verificar nessa função
/// @param point Ponto para se testar colisão
/// @return Se true, então houve colisão do ponto com as bordas do grid ou partes da cobra; Se false, não houve colisão 
bool SnakeGame::isCollision(Vector2 point) const {
    // Se houve colisão com as bordas do grid, retorna true
    if (point.x < 0 || point.x >= gridWidth || point.y < 0 || point.y >= gridHeight) return true;

    // Caso não haja colisão com as bordas, verifica se houve colisão com as partes da cobra
    for (const Vector2 part : snake) {
        if (part.x == point.x && part.y == point.y) return true;
    }

    return false; // Se chegou até aqui, não houve colisão nenhuma
}


/// @brief Pega a nova direção da cobra dada uma ação (a)
/// @param action Ação escolhida
/// @return Retorna a nova direção da cobra
Direction SnakeGame::getNewDirection(Action action) const {
    int direction = static_cast<int>(currentDirection); // Força o cast de inteiro para o enum de direções  

    // Se a direção foe para virar a direita
    if (action == Action::TURN_RIGHT) { // Usa de uma tática de game-dev pelo o sentido horário, rotação clockwise
        return static_cast<Direction>((direction + 1) % 4); // Cima -> direita -> baixo -> esquerda -> cima 
    }
    else if (action == Action::TURN_LEFT) { // Pela mesma tácica, usa o sentido anti-horário do relógio 
        return static_cast<Direction>((direction + 3) % 4); // Cima -> esquerda -> baixo -> direita -> cima
    }

    // Se não for virar a direita ou esquerda, a ação que sobre é a frente, i.e., mantém a mesma direção
    return currentDirection;
}


/// @brief Pega o vizinho, i.e., o próximo tile (ponto (x,y)) onde a cabeça da cobra irá visitar 
/// @param direction 
/// @return 
Vector2 SnakeGame::getNeighbor(Direction direction) const {
    Vector2 head = snake.front(); // Pega a cabeça da cobra (a sua coordenada, no caso)

    switch (direction) { // Dependendo da direção da cobra, pega o ponto orientado pela cabeça da cobra
        case Direction::UP: return { head.x, head.y - 1 }; // Pega o ponto acima da cabeça 
        case Direction::DOWN: return { head.x, head.y + 1}; // Pega o ponto abaixo da cabeça
        case Direction::LEFT: return { head.x - 1, head.y}; // Pega o ponto à esquerda da cabeça
        case Direction::RIGHT: return { head.x + 1, head.y}; // Pega o ponto à direita da cabeça
    }

    return head; // Retorna a nova lozalização da cabeça da cobra
}


/// @brief Reseta o jogo, i.e., reinicializa os parâmetros do jogo
/// @return Retorna o estado do começo do jogo
int SnakeGame::reset() {
    score = 0; // Reseta a pontuação
    isGameOver = false; // Jogo ainda não acabou
    currentDirection = Direction::RIGHT; // Inicializa a direção da cobra no sentido direito

    float startPosX = gridWidth / 2; // Horizontalmente, a cobra inicia-se na metade da grid 
    float startPosY = gridHeight / 2; // Verticalmente, a cobra inicia-se na metade do grid

    snake.clear(); // Limpa totalmente a cobra (i.e., a sua lista de coordenadas no deque)
    snake.push_back({startPosX, startPosY}); // Adiciona a sua coordenada inicial da cabeça no centro do grid
    snake.push_back({startPosX - 1, startPosY}); // Adiciona o seu corpo na direita da sua cabeça
    snake.push_back({startPosX - 2, startPosY}); // Adiciona o seu rabo na direita do seu corpo

    placeFood(); // Inicializa aleatoriamente a comida

    return getState(); // Retorna o estado inicial do gjogo
}


/// @brief Gera um step/instante de tempo (i.e., movimento) no jogo 
/// @param action A ação tomada para aquele instante de tempo
/// @return Retorna o resultado daquele instante de tempo, i.e., (s', r, d, g)
StepResult SnakeGame::step(Action action) {
    currentDirection = getNewDirection(action); // Pega a nova direção da cobra com base na ação tomada

    Vector2 head = snake.front(); // Pega a cabeça da cobra
    Vector2 newHead = getNeighbor(currentDirection); // Pega a vizinhança para onde ela se movimentará

    if (isCollision(newHead)) { // Se houve colisão nessa movimentação
        isGameOver = true; // Jogo acabou
        return { getState(), -10.0f, true, score }; // Retorna uma tupla com recompensa negativa 
    }

    snake.push_front(newHead); // Se não houve colisão, então podemos movimentar

    float reward; // Calcula a nova recompensa
    if (newHead.x == food.x && newHead.y == food.y) { // Se ela comeu a comida
        score++; // Score é incrementado (quantidade de comida)
        reward = 10.0f; // Recebe 10 de recompensa

        if (snake.size() == gridWidth * gridHeight) { // Se a cobra atingiu o máximo de tamanho
            isGameOver = true; // Venceu o jogo, logo fim de jogo
            return { getState(), 100.0f, true, score }; // Retorna o estado de vitória
        }

        placeFood(); // Gera o novo local de comida
    }
    else { // Se não comeu a comida no movimento
        snake.pop_back(); // A cauda é removida (já que ela se movimenta 1 passo)
        reward = -0.1f; // Recebe uma recompensa negativa 
    }

    return { getState(), reward, false, score }; // Retorna o estado do passo
}


/// @brief Renderiza o jogo, i.e., desenha a interface gráfica
void SnakeGame::render() const {
    // Percorre pelos grids do jogo 
    for (int row = 0; row < gridHeight; row++) { 
        for (int col = 0; col < gridWidth; col++) {
            // Faz uma projeção de 10x10 -> 103x65 = 1030x650 (o que faz sentido para uma tela de 1280x720) 
            int posX = 25 + col * TILE_WIDTH;
            int posY = 25 + row * TILE_HEIGHT; 
            // Desenha a tela e as grades
            DrawRectangle(posX, posY, TILE_WIDTH, TILE_HEIGHT, GREEN); 
            DrawRectangleLinesEx({(float) posX, (float) posY, (float)TILE_WIDTH, (float)TILE_HEIGHT}, 0.5, BLACK);
        }
    }

    // Desenha o círculo amarelo da comida
    int foodPosX = 25 + food.x * TILE_WIDTH;
    int foodPosY = 25 + food.y * TILE_HEIGHT;
    DrawCircle(foodPosX + TILE_WIDTH / 2, foodPosY + TILE_HEIGHT / 2, 25, YELLOW);

    // Desenha o retângulo do corpo verde-escuro da cobra
    for (int i = 1; i < snake.size(); i++) {
        int posX = 25 + snake[i].x * TILE_WIDTH;
        int posY = 25 + snake[i].y * TILE_HEIGHT; 
        DrawRectangle(posX, posY, TILE_WIDTH, TILE_HEIGHT, DARKGREEN);
    }

    // Desenha o retângulo da cabeça da prova
    int headX = 25 + snake.front().x * TILE_WIDTH;
    int headY = 25 + snake.front().y * TILE_HEIGHT;
    DrawRectangle(headX, headY, TILE_WIDTH, TILE_HEIGHT, DARKGREEN);
    // Incluindo seus dois olhos
    DrawCircle(headX + TILE_WIDTH / 4, headY + TILE_HEIGHT / 4, 10, RAYWHITE);
    DrawCircle(headX + 1 + TILE_WIDTH / 4, headY + 1 + TILE_HEIGHT / 4, 5, BLACK);
    DrawCircle(headX + 3 * TILE_WIDTH / 4, headY + TILE_HEIGHT / 4, 10, RAYWHITE);
    DrawCircle(headX + 1 + 3 * TILE_WIDTH / 4, headY + 1 + TILE_HEIGHT / 4, 5, BLACK);

}




/// @brief Pega o estado do jogo atual 
/// @return Retorna um inteiro de 9 bits, i.e., 2^9 = 512 valores possíveis
int SnakeGame::getState() const {
    Vector2 head = snake.front(); // Pega a cabeça da cobra 

    // Verifica as 3 flags para perigo
    bool dangerStraight = isCollision(getNeighbor(currentDirection)); // À frente
    bool dangerLeft = isCollision(getNeighbor(getNewDirection(Action::TURN_LEFT))); // À esquerda
    bool dangerRight = isCollision(getNeighbor(getNewDirection(Action::TURN_RIGHT))); // Ou à direita

    // Verificas as 4 flags para localização da comida com base na cabeça da cobra
    bool foodLeft = food.x < head.x; // À esquerda
    bool foodRight = food.x > head.x; // À direita 
    bool foodUp = food.y < head.y; // Acima
    bool foodDown = food.y > head.y; // Ou abaixo

    int state = 0; // Constrói binariamente o valor do estado 
    if (dangerStraight) state |= (1 << 0); // Bit 0
    if (dangerLeft) state |= (1 << 1); // Bit 1
    if (dangerRight) state |= (1 << 2); // Bit 2
    if (foodLeft) state |= (1 << 3); // Bit 3
    if (foodRight) state |= (1 << 4); // Bit 4
    if (foodUp) state |= (1 << 5); // Bit 5
    if (foodDown) state |= (1 << 6); // Bit 6
    // Além das flags, adiciona os 2 bits da direção, i.e., 00 (cima), 01 (direita), 10 (baixo), 11 (esquerda) 
    state |= (static_cast<int>(currentDirection) << 7); // Bit 6 + 2 bits = 8 bits, i.e., 2^9

    return state; // Retorna o valor binário
}

/// @brief Cria um valor para as features do jogo (usado na função de aproximação)
/// @return Retorna um  
vector<float> SnakeGame::getFeatures() const {
    Vector2 head = snake.front(); // Pega a cabeça da cobra
    
    // Define um valor de 1 ou 0 para qualquer perigo que a cobra possa ter
    float dangerStraight = isCollision(getNeighbor(currentDirection)) ? 1.0f : 0.0f; // À frente
    float dangerLeft = isCollision(getNeighbor(getNewDirection(Action::TURN_LEFT))) ? 1.0f : 0.0f; // À esquerda 
    float dangerRight = isCollision(getNeighbor(getNewDirection(Action::TURN_RIGHT))) ? 1.0f : 0.0f; // À direita

    // Calcula a diferença em relação à comida (usado para orientar a direção da cobra)
    float dx = food.x - head.x; // Na horizontal (x) 
    float dy = food.y - head.y; // Na vertical (y)

    // Inicializa os próximos valores para a posição da comida  
    float foodAhead = 0.0f, foodBehind = 0.0f, foodLeft = 0.0f, foodRight = 0.0f;
    switch (currentDirection) { // Verifica onde a comida se contra, geometricamente
        case Direction::UP: // Na direção de subindo 
            if (dy < 0) foodAhead  = 1.0f;
            if (dy > 0) foodBehind = 1.0f;
            if (dx < 0) foodLeft   = 1.0f;
            if (dx > 0) foodRight  = 1.0f;
        break;

        case Direction::DOWN: // Na direção de descendo
            if (dy > 0) foodAhead  = 1.0f;
            if (dy < 0) foodBehind = 1.0f;
            if (dx > 0) foodLeft   = 1.0f;
            if (dx < 0) foodRight  = 1.0f;
        break;

        case Direction::LEFT: // Na direção da esquerda
            if (dx < 0) foodAhead  = 1.0f;
            if (dx > 0) foodBehind = 1.0f;
            if (dy > 0) foodLeft   = 1.0f;
            if (dy < 0) foodRight  = 1.0f;
        break;

        case Direction::RIGHT: // Na direção da direita
            if (dx > 0) foodAhead  = 1.0f;
            if (dx < 0) foodBehind = 1.0f;
            if (dy < 0) foodLeft   = 1.0f;
            if (dy > 0) foodRight  = 1.0f;
            break;
    }
    // Retorna a lista de features 
    return { dangerStraight, dangerLeft, dangerRight,foodAhead, foodBehind, foodLeft, foodRight };
}


/// @brief Desenha a tela de menu do jogo 
/// @param mousePos 
void DrawMenuScreen(Vector2 mousePos) {
    DrawText("Snake Game!", 25, 14, 58, DARKGREEN); // Título do menu
    DrawText("Choose your algorithm:", 25, 14*2 + 58, 36, BLACK); // Subtítulo do menu

    // Desenha o botão de Q-learning Tabular
    Color colorBtnQLearning = LIGHTGRAY; // Cor para o botão de Q-learning
    if (CheckCollisionPointRec(mousePos, BTN_QLEARNING)) colorBtnQLearning = GRAY; // Hover para o botão
    DrawRectangleRec(BTN_QLEARNING, colorBtnQLearning); // Desenha o retangulo
    DrawText("Q-Learning Tabular", BTN_QLEARNING.x + 24, BTN_QLEARNING.y + 14, 36, BLACK); // Desenha o texto

    // Desenha o botão de Q-learning por função de aproximação
    Color colorBtnQLearningFunc = LIGHTGRAY; // Cor para o botão de Q-learning
    if (CheckCollisionPointRec(mousePos, BTN_QLEARNINGFUNC)) colorBtnQLearningFunc = GRAY; // Hover para o botão
    DrawRectangleRec(BTN_QLEARNINGFUNC, colorBtnQLearningFunc); // Desenha o retangulo
    DrawText("Q-Learning Função", BTN_QLEARNINGFUNC.x + 24, BTN_QLEARNINGFUNC.y + 14, 36, BLACK); // Desenha o texto


}