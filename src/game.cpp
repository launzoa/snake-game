#include "game.hpp"
using namespace std;

SnakeGame::SnakeGame(int width, int height) : gridWidth(width), gridHeight(height) {
    reset();
}

int SnakeGame::reset() {
    score = 0;
    isGameOver = false;
    currentDirection = Direction::RIGHT;

    float startPosX = gridWidth / 2; 
    float startPosY = gridHeight / 2;

    snake.clear();
    snake.push_back({startPosX, startPosY});
    snake.push_back({startPosX - 1, startPosY});
    snake.push_back({startPosX - 2, startPosY});

    placeFood();

    return getState();
}

void SnakeGame::placeFood() {
    while (true) {
        float posX = GetRandomValue(0, gridWidth-1);
        float posY = GetRandomValue(0, gridHeight-1);
        Vector2 randomPoint = {posX, posY}; 

        bool onSnake = false;
        for (const Vector2 part : snake) {
            if (part.x == randomPoint.x && part.y == randomPoint.y) {
                onSnake = true;
                break;
            }
        }

        if (!onSnake) {
            food = randomPoint;
            break;
        }
    }
}

bool SnakeGame::isCollision(Vector2 point) const {
    if (point.x < 0 || point.x >= gridWidth || point.y < 0 || point.y >= gridHeight) return true;

    for (const Vector2 part : snake) {
        if (part.x == point.x && part.y == point.y) return true;
    }

    return false;
}


Vector2 SnakeGame::getNeighbor(Direction direction) const {
    Vector2 head = snake.front(); 

    switch (direction) {
        case Direction::UP: return { head.x, head.y - 1 };

        case Direction::DOWN: return { head.x, head.y + 1};

        case Direction::LEFT: return { head.x - 1, head.y};

        case Direction::RIGHT: return { head.x + 1, head.y};
    }

    return head;
}

Direction SnakeGame::getNewDirection(Action action) const {
    if (action == Action::STRAIGHT) return currentDirection;

    if (action == Action::TURN_RIGHT) {
        switch (currentDirection) {
            case Direction::UP: return Direction::RIGHT;
            case Direction::RIGHT: return Direction::DOWN;
            case Direction::DOWN: return Direction::LEFT;
            case Direction::LEFT: return Direction::UP;
        }
    } 
    else if (action == Action::TURN_LEFT) {
        switch (currentDirection) {
            case Direction::UP: return Direction::LEFT;
            case Direction::LEFT: return Direction::DOWN;
            case Direction::DOWN: return Direction::RIGHT;
            case Direction::RIGHT: return Direction::UP;
        }
    }

    return currentDirection;
}


StepResult SnakeGame::step(Action action) {
    currentDirection = getNewDirection(action); 

    Vector2 head = snake.front();
    Vector2 newHead = head;

    switch(currentDirection) {
        case Direction::UP: newHead.y--; 
        break;

        case Direction::DOWN: newHead.y++; 
        break;
            
        case Direction::LEFT: newHead.x--;
        break;

        case Direction::RIGHT: newHead.x++;
        break;
    }

    if (isCollision(newHead)) {
        isGameOver = true;
        return { getState(), -10.0f, true, score };
    }

    snake.push_front(newHead);

    float reward;
    if (newHead.x == food.x && newHead.y == food.y) {
        score++;
        reward = 10.0f;

        if (snake.size() == gridWidth * gridHeight) {
            isGameOver = true;
            return { getState(), 100.0f, true, score };
        }

        placeFood();
    }
    else {
        snake.pop_back();
        reward = -0.01f;
    }

    return { getState(), reward, false, score };
}


int SnakeGame::getState() const {
    Vector2 head = snake.front(); 

    bool dangerStraight = isCollision(getNeighbor(currentDirection));
    bool dangerLeft = isCollision(getNeighbor(getNewDirection(Action::TURN_LEFT)));
    bool dangerRight = isCollision(getNeighbor(getNewDirection(Action::TURN_RIGHT)));

    bool foodLeft = food.x < head.x;
    bool foodRight = food.x > head.x;
    bool foodUp = food.y < head.y;
    bool foodDown = food.y > head.y;

    int state = 0;
    if (dangerStraight) state |= (1 << 0);
    if (dangerLeft) state |= (1 << 1);
    if (dangerRight) state |= (1 << 2);
    if (foodLeft) state |= (1 << 3);
    if (foodRight) state |= (1 << 4);
    if (foodUp) state |= (1 << 5);
    if (foodDown) state |= (1 << 6);
    state |= (static_cast<int>(currentDirection) << 7);

    return state;
}

void SnakeGame::render() const {
    for (int row = 0; row < gridHeight; row++) {
        for (int col = 0; col < gridWidth; col++) {
            int posX = 25 + col * TILE_WIDTH;
            int posY = 25 + row * TILE_HEIGHT; 

            DrawRectangle(posX, posY, TILE_WIDTH, TILE_HEIGHT, GREEN);
            DrawRectangleLinesEx({posX, posY, TILE_WIDTH, TILE_HEIGHT}, 0.5, BLACK);
        }
    }

    int foodPosX = 25 + food.x * TILE_WIDTH;
    int foodPosY = 25 + food.y * TILE_HEIGHT;
    DrawCircle(foodPosX + TILE_WIDTH / 2, foodPosY + TILE_HEIGHT / 2, 25, YELLOW);

    for (int i = 1; i < snake.size(); i++) {
        int posX = 25 + snake[i].x * TILE_WIDTH;
        int posY = 25 + snake[i].y * TILE_HEIGHT; 
        DrawRectangle(posX, posY, TILE_WIDTH, TILE_HEIGHT, DARKGREEN);
    }

    int headX = 25 + snake.front().x * TILE_WIDTH;
    int headY = 25 + snake.front().y * TILE_HEIGHT;
    DrawRectangle(headX, headY, TILE_WIDTH, TILE_HEIGHT, DARKGREEN);
    DrawCircle(headX + TILE_WIDTH / 4, headY + TILE_HEIGHT / 4, 10, RAYWHITE);
    DrawCircle(headX + 1 + TILE_WIDTH / 4, headY + 1 + TILE_HEIGHT / 4, 5, BLACK);
    DrawCircle(headX + 3 * TILE_WIDTH / 4, headY + TILE_HEIGHT / 4, 10, RAYWHITE);
    DrawCircle(headX + 1 + 3 * TILE_WIDTH / 4, headY + 1 + TILE_HEIGHT / 4, 5, BLACK);
}


/// @brief 
/// @param mousePos 
void DrawMenuScreen(Vector2 mousePos) {
    DrawText("Snake Game!", 25, 14, 58, DARKGREEN);
    DrawText("Choose your algorithm:", 25, 14*2 + 58, 36, BLACK);

    Color colorBtnQLearning = LIGHTGRAY;
    if (CheckCollisionPointRec(mousePos, BTN_QLEARNING)) colorBtnQLearning = GRAY;
    DrawRectangleRec(BTN_QLEARNING, colorBtnQLearning);
    DrawText("Q-Learning", BTN_QLEARNING.x + 24, BTN_QLEARNING.y + 14, 36, BLACK);

}

vector<string> loadGrid(const string& filePath) {
    vector<string> grid;
    ifstream file(filePath);

    if (!file.is_open()){ 
        cerr << "[Error] not possible open the file: " << filePath << "\n"; 
        return grid;
    }

    string line; 
    while (getline(file, line)) {
        if (!line.empty()) grid.push_back(line);
    }

    file.close();
    return grid;
} 