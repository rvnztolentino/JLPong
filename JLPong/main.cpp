#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>

const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 576;
const float PADDLE_WIDTH = 10.0f;
const float PADDLE_HEIGHT = 80.0f;
const float PADDLE_SPEED = 600.0f;
const float BALL_SIZE = 10.0f;
const float MIN_BALL_SPEED = 520.0f;
const float MAX_BALL_SPEED = 670.0f;

enum GameState {
    MENU,
    GAMEPLAY
};

GameState currentState = MENU;

float P1_PaddleY = WINDOW_HEIGHT / 2.0f - PADDLE_HEIGHT / 2.0f;
float P2_PaddleY = WINDOW_HEIGHT / 2.0f - PADDLE_HEIGHT / 2.0f;
float ballX = WINDOW_WIDTH / 2.0f;
float ballY = WINDOW_HEIGHT / 2.0f;
float ballVelX = MAX_BALL_SPEED;
float ballVelY = MAX_BALL_SPEED;

int player1Score = 0;
int player2Score = 0;
bool spaceWasPressed = false;

int selectedMenuItem = 0;
bool enterWasPressed = false;
bool upKeyWasPressed = false;
bool downKeyWasPressed = false;
std::vector<std::string> menuItems = { "START", "EXIT" };

float randFloat(float min, float max) {
    return min + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (max - min);
}

void drawRect(float x, float y, float width, float height) {
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

void drawChar(char c, float x, float y, float size) {
    float thickness = size / 5.0f;

    switch (c) {
    case 'A':
        glBegin(GL_TRIANGLES);
        glVertex2f(x + size / 2, y);
        glVertex2f(x, y + size);
        glVertex2f(x + size, y + size);
        glEnd();
        glBegin(GL_QUADS);
        glVertex2f(x + size / 4, y + size / 2);
        glVertex2f(x + 3 * size / 4, y + size / 2);
        glVertex2f(x + 3 * size / 4, y + size / 2 + thickness);
        glVertex2f(x + size / 4, y + size / 2 + thickness);
        glEnd();
        break;
    case 'E':
        drawRect(x, y, thickness, size); 
        drawRect(x, y, size, thickness);
        drawRect(x, y + size / 2 - thickness / 2, size, thickness);
        drawRect(x, y + size - thickness, size, thickness); 
        break;
    case 'G':
        drawRect(x, y, thickness, size);
        drawRect(x, y, size, thickness);  
        drawRect(x, y + size - thickness, size, thickness);
        drawRect(x + size - thickness, y + size / 2, thickness, size / 2); 
        drawRect(x + size / 2, y + size / 2, size / 2, thickness); 
        break;
    case 'I':
        drawRect(x, y, size, thickness);
        drawRect(x + size / 2 - thickness / 2, y, thickness, size); 
        drawRect(x, y + size - thickness, size, thickness); 
        break;
    case 'J':
        drawRect(x + size - thickness, y, thickness, size - thickness); 
        drawRect(x, y + size - thickness, size, thickness);  
        drawRect(x, y + size / 2, thickness, size / 2); 
        break;
    case 'L':
        drawRect(x, y, thickness, size);  
        drawRect(x, y + size - thickness, size, thickness);
        break;
    case 'N':
        drawRect(x, y, thickness, size); 
        drawRect(x + size - thickness, y, thickness, size);
        // diagonal line (approximated with small rectangles)
        for (int i = 0; i < 10; i++) {
            float yPos = y + i * (size / 10);
            float xPos = x + i * (size / 10);
            drawRect(xPos, yPos, thickness, thickness);
        }
        break;
    case 'O':
        drawRect(x, y, thickness, size); 
        drawRect(x, y, size, thickness); 
        drawRect(x + size - thickness, y, thickness, size); 
        drawRect(x, y + size - thickness, size, thickness); 
        break;
    case 'P':
        drawRect(x, y, thickness, size);
        drawRect(x, y, size, thickness); 
        drawRect(x + size - thickness, y, thickness, size / 2); 
        drawRect(x, y + size / 2, size, thickness); 
        break;
    case 'R':
        drawRect(x, y, thickness, size); 
        drawRect(x, y, size, thickness); 
        drawRect(x + size - thickness, y, thickness, size / 2); 
        drawRect(x, y + size / 2, size, thickness); 

        // diagonal line for the leg (approximated with small rectangles)
        for (int i = 0; i < 5; i++) {
            float yPos = y + size / 2 + i * (size / 2 / 5);
            float xPos = x + size / 2 + i * (size / 2 / 5);
            drawRect(xPos, yPos, thickness, thickness);
        }
        break;
    case 'S':
        drawRect(x, y, size, thickness);  
        drawRect(x, y, thickness, size / 2);  
        drawRect(x, y + size / 2 - thickness / 2, size, thickness);
        drawRect(x + size - thickness, y + size / 2, thickness, size / 2); 
        drawRect(x, y + size - thickness, size, thickness); 
        break;
    case 'T':
        drawRect(x, y, size, thickness);
        drawRect(x + size / 2 - thickness / 2, y, thickness, size);
        break;
    case 'X':
        // diagonal lines (approximated with small rectangles)
        for (int i = 0; i < 10; i++) {
            float yPos1 = y + i * (size / 10);
            float xPos1 = x + i * (size / 10);
            float yPos2 = y + i * (size / 10);
            float xPos2 = x + size - i * (size / 10) - thickness;
            drawRect(xPos1, yPos1, thickness, thickness);
            drawRect(xPos2, yPos2, thickness, thickness);
        }
        break;
    case ' ':
        // space - do nothing
        break;
    default:
        // for any other character, draw a rectangle as placeholder
        drawRect(x, y, size, size);
        break;
    }
}

void drawText(const std::string& text, float x, float y, float charSize, float spacing) {
    float currentX = x;

    for (size_t i = 0; i < text.size(); i++) {
        drawChar(text[i], currentX, y, charSize);
        currentX += charSize + spacing;
    }
}

void drawDigit(int digit, float x, float y, float size) {
    float thickness = size / 5.0f;

    // segments for different digits (1 = on, 0 = off)
    // segments are: top, top-right, bottom-right, bottom, bottom-left, top-left, middle
    const int segments[10][7] = {
        {1, 1, 1, 1, 1, 1, 0},  // 0
        {0, 1, 1, 0, 0, 0, 0},  // 1
        {1, 1, 0, 1, 1, 0, 1},  // 2
        {1, 1, 1, 1, 0, 0, 1},  // 3
        {0, 1, 1, 0, 0, 1, 1},  // 4
        {1, 0, 1, 1, 0, 1, 1},  // 5
        {1, 0, 1, 1, 1, 1, 1},  // 6
        {1, 1, 1, 0, 0, 0, 0},  // 7
        {1, 1, 1, 1, 1, 1, 1},  // 8
        {1, 1, 1, 1, 0, 1, 1}   // 9
    };

    // draw segments based on the digit
    if (digit >= 0 && digit <= 9) {
        if (segments[digit][0])
            drawRect(x + thickness, y, size - 2 * thickness, thickness); // top
        if (segments[digit][1])
            drawRect(x + size - thickness, y, thickness, size / 2); // top-right
        if (segments[digit][2])
            drawRect(x + size - thickness, y + size / 2, thickness, size / 2); // bottom-right
        if (segments[digit][3])
            drawRect(x + thickness, y + size - thickness, size - 2 * thickness, thickness); // bottom
        if (segments[digit][4])
            drawRect(x, y + size / 2, thickness, size / 2); // bottom-left
        if (segments[digit][5])
            drawRect(x, y, thickness, size / 2); // top-left
        if (segments[digit][6])
            drawRect(x + thickness, y + size / 2 - thickness / 2, size - 2 * thickness, thickness); // middle
    }
}

void drawScore(int score, float x, float y, float digitSize, float spacing) {
    std::string scoreStr = std::to_string(score); // converts score to string to handle multiple digits
    float currentX = x;

    for (char digit : scoreStr) {
        drawDigit(digit - '0', currentX, y, digitSize);
        currentX += digitSize + spacing;
    }
}

void updatePaddle(float& paddleY, float deltaTime, int upKey, int downKey) {
    if (glfwGetKey(glfwGetCurrentContext(), upKey) == GLFW_PRESS) {
        paddleY -= PADDLE_SPEED * deltaTime;
        if (paddleY < 0) paddleY = 0;
    }
    if (glfwGetKey(glfwGetCurrentContext(), downKey) == GLFW_PRESS) {
        paddleY += PADDLE_SPEED * deltaTime;
        if (paddleY + PADDLE_HEIGHT > WINDOW_HEIGHT) paddleY = WINDOW_HEIGHT - PADDLE_HEIGHT;
    }
}

void randomizeBallVelocity(bool isLeftPaddle) {
    float speed = randFloat(MIN_BALL_SPEED, MAX_BALL_SPEED);

    if (isLeftPaddle) {
        ballVelX = speed;
    }
    else {
        ballVelX = -speed;
    }

    ballVelY = randFloat(-MAX_BALL_SPEED, MAX_BALL_SPEED); // randomizes Y velocity between -MAX and +MAX
}

void updateBall(float deltaTime) {
    ballX += ballVelX * deltaTime;
    ballY += ballVelY * deltaTime;

    // wall collisions (top and bottom)
    if (ballY <= 10.0f || ballY + BALL_SIZE >= WINDOW_HEIGHT - 10.0f) {
        ballVelY = -ballVelY;
    }

    // left paddle collision
    if (ballX <= PADDLE_WIDTH && ballY + BALL_SIZE >= P1_PaddleY && ballY <= P1_PaddleY + PADDLE_HEIGHT) {
        randomizeBallVelocity(true);
    }

    // right paddle collision
    if (ballX + BALL_SIZE >= WINDOW_WIDTH - PADDLE_WIDTH &&
        ballY + BALL_SIZE >= P2_PaddleY && ballY <= P2_PaddleY + PADDLE_HEIGHT) {
        randomizeBallVelocity(false);
    }

    // if P1 losses
    if (ballX <= 0) {
        player2Score++;
        ballX = WINDOW_WIDTH / 2.0f + 400.0f;
        ballY = WINDOW_HEIGHT / 2.0f;

        // randomize initial direction (toward player 1)
        float speed = randFloat(MIN_BALL_SPEED, MAX_BALL_SPEED);
        ballVelX = -speed;
        ballVelY = randFloat(-speed / 2, speed / 2);
    }
    // if P2 losses
    else if (ballX + BALL_SIZE >= WINDOW_WIDTH) {
        player1Score++;
        ballX = WINDOW_WIDTH / 2.0f - 400.0f;
        ballY = WINDOW_HEIGHT / 2.0f;

        // randomize initial direction (toward player 2)
        float speed = randFloat(MIN_BALL_SPEED, MAX_BALL_SPEED);
        ballVelX = speed;
        ballVelY = randFloat(-speed / 2, speed / 2);
    }
}

void checkResetScore() {
    bool spaceIsPressed = glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_SPACE) == GLFW_PRESS;
    if (spaceIsPressed && !spaceWasPressed) {
        player1Score = 0;
        player2Score = 0;

        ballX = WINDOW_WIDTH / 2.0f;
        ballY = WINDOW_HEIGHT / 2.0f;

        float initialSpeed = randFloat(MIN_BALL_SPEED, MAX_BALL_SPEED);
        ballVelX = (std::rand() % 2 == 0 ? -1.0f : 1.0f) * initialSpeed; // randomly choose initial X direction (left or right)
        if (ballVelX >= 0) {
            ballX -= 400.0f;
        }
        else {
            ballX += 400.0f;
        }
        std::cout << "RESET" << "\n";
        std::cout << "BALL_POS: " << ballVelX << "\n";
        ballVelY = randFloat(-initialSpeed / 2.0f, initialSpeed / 2.0f); // more modest initial Y velocity
    }

    spaceWasPressed = spaceIsPressed;
}

void initGame() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)) & 0xFFFFFFFF);

    player1Score = 0;
    player2Score = 0;

    P1_PaddleY = WINDOW_HEIGHT / 2.0f - PADDLE_HEIGHT / 2.0f;
    P2_PaddleY = WINDOW_HEIGHT / 2.0f - PADDLE_HEIGHT / 2.0f;

    ballX = WINDOW_WIDTH / 2.0f;
    ballY = WINDOW_HEIGHT / 2.0f;

    float initialSpeed = randFloat(MIN_BALL_SPEED, MAX_BALL_SPEED);
    ballVelX = (std::rand() % 2 == 0 ? -1.0f : 1.0f) * initialSpeed; // randomly choose initial X direction (left or right)
    if (ballVelX >= 0) {
        ballX -= 400.0f;
    }
    else {
        ballX += 400.0f;
    }
    std::cout << "INITIALIZE" << "\n";
    std::cout << "BALL_POS: " << ballVelX << "\n";
    ballVelY = randFloat(-initialSpeed / 2.0f, initialSpeed / 2.0f); // more modest initial Y velocity
}

void updateMenu() {
    bool upIsPressed = glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_UP) == GLFW_PRESS;
    if (upIsPressed && !upKeyWasPressed) {
        int menuSize = static_cast<int>(menuItems.size()); // cast menuItems.size() to avoid size_t to int conversion issues
        selectedMenuItem = (selectedMenuItem - 1 + menuSize) % menuSize;
    }
    upKeyWasPressed = upIsPressed;

    bool downIsPressed = glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_DOWN) == GLFW_PRESS;
    if (downIsPressed && !downKeyWasPressed) {
        int menuSize = static_cast<int>(menuItems.size()); // cast menuItems.size() to avoid size_t to int conversion issues
        selectedMenuItem = (selectedMenuItem + 1) % menuSize;
    }
    downKeyWasPressed = downIsPressed;

    bool enterIsPressed = glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_ENTER) == GLFW_PRESS;
    if (enterIsPressed && !enterWasPressed) {
        if (selectedMenuItem == 0) {
            currentState = GAMEPLAY;
            std::cout << "GAME START" << "\n";
            initGame();
        }
        else if (selectedMenuItem == 1) {
            glfwSetWindowShouldClose(glfwGetCurrentContext(), GLFW_TRUE);
        }
    }
    enterWasPressed = enterIsPressed;
}

void displayMenu() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -1, 1);

    glColor3f(0.0f, 1.0f, 0.0f);
    drawText("JLPONG", WINDOW_WIDTH / 2.0f - 120.0f, 80.0f, 40.0f, 5.0f);

    glColor3f(1.0f, 1.0f, 1.0f);

    for (int i = 0; i < static_cast<int>(menuItems.size()); i++) {
        if (i == selectedMenuItem) {
            drawText("> " + menuItems[i], WINDOW_WIDTH / 2.0f - 80.0f, 200.0f + i * 60.0f, 30.0f, 5.0f);
        }
        else {
            drawText("  " + menuItems[i], WINDOW_WIDTH / 2.0f - 80.0f, 200.0f + i * 60.0f, 30.0f, 5.0f);
        }
    }

    drawText("USE UP/DOWN ARROWS TO SELECT", WINDOW_WIDTH / 2.0f - 230.0f, WINDOW_HEIGHT - 100.0f, 15.0f, 2.0f);
    drawText("PRESS ENTER TO CONFIRM", WINDOW_WIDTH / 2.0f - 160.0f, WINDOW_HEIGHT - 70.0f, 15.0f, 2.0f);
}

void displayGameplay() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -1, 1);

    glColor3f(1.0f, 1.0f, 1.0f);

    for (int y = 0; y < WINDOW_HEIGHT; y += 20) {
        drawRect(WINDOW_WIDTH / 2.0f - 1.0f, static_cast<float>(y), 2.0f, 10.0f);
    }

    drawRect(0.0f, P1_PaddleY, PADDLE_WIDTH, PADDLE_HEIGHT);
    drawRect(WINDOW_WIDTH - PADDLE_WIDTH, P2_PaddleY, PADDLE_WIDTH, PADDLE_HEIGHT);
    drawRect(ballX, ballY, BALL_SIZE, BALL_SIZE);

    float digitSize = 30.0f;
    float spacing = 5.0f;

    drawScore(player1Score, WINDOW_WIDTH / 4.0f - digitSize, 200.0f, digitSize, spacing);
    drawScore(player2Score, WINDOW_WIDTH * 3.0f / 4.0f - digitSize, 200.0f, digitSize, spacing);

    drawText("PRESS SPACE TO RESET SCORE", WINDOW_WIDTH / 2.0f - 190.0f, WINDOW_HEIGHT - 30.0f, 15.0f, 2.0f);
    drawText("ESC TO RETURN TO MENU", WINDOW_WIDTH / 2.0f - 160.0f, WINDOW_HEIGHT - 60.0f, 15.0f, 2.0f);
}

int main(void) {
    GLFWwindow* window;
    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(1024, 576, "JLPong by @rvnztolentino", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    std::srand(static_cast<unsigned int>(std::time(nullptr)) & 0xFFFFFFFF); // explicitly cast to avoid potential data loss

    // main game loop
    double previousFrame = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        double currentFrame = glfwGetTime();
        float deltaTime = static_cast<float>(currentFrame - previousFrame); // explicit cast to float

        previousFrame = currentFrame;

        if (currentState == GAMEPLAY && glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            std::cout << "BACK TO MENU" << "\n";
            currentState = MENU;
        }

        if (currentState == MENU) {
            updateMenu();
            displayMenu();
        }
        else if (currentState == GAMEPLAY) {
            updatePaddle(P1_PaddleY, deltaTime, GLFW_KEY_W, GLFW_KEY_S); // P1
            updatePaddle(P2_PaddleY, deltaTime, GLFW_KEY_UP, GLFW_KEY_DOWN); // P2
            updateBall(deltaTime);
            checkResetScore();
            displayGameplay();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}