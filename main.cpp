#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <deque>
#include <ctime>
#include <cstdlib>

const int CELL_SIZE = 20;
const int WIDTH = 800;
const int HEIGHT = 600;

enum Direction { Up, Down, Left, Right };

struct Segment {
    int x, y;
};

class SnakeGame {
public:
    SnakeGame();
    void run();

private:
    sf::RenderWindow window;
    sf::SoundBuffer eatBuffer, gameOverBuffer;
    sf::Sound eatSound, gameOverSound;
    sf::Font font;
    sf::Text scoreText, gameOverText;

    std::deque<Segment> snake;
    Direction dir;
    Segment food;
    bool gameOver;
    int score;
    float speed;

    sf::Clock clock;
    float timer;

    void reset();
    void processEvents();
    void update();
    void render();
    void move();
    void generateFood();
    void checkCollision();
};

SnakeGame::SnakeGame()
    : window(sf::VideoMode(WIDTH, HEIGHT), "Snake Game"), gameOver(false), score(0), speed(0.1f), timer(0) {
    window.setFramerateLimit(60);

    eatBuffer.loadFromFile("assets/eat.wav");
    gameOverBuffer.loadFromFile("assets/gameover.wav");
    eatSound.setBuffer(eatBuffer);
    gameOverSound.setBuffer(gameOverBuffer);

    font.loadFromFile("assets/sansation.ttf");
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 10);

    gameOverText.setFont(font);
    gameOverText.setCharacterSize(36);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setString("Game Over! Press R to Restart");
    gameOverText.setPosition(WIDTH / 4, HEIGHT / 2);

    reset();
}

void SnakeGame::reset() {
    snake.clear();
    snake.push_back({10, 10});
    dir = Right;
    score = 0;
    speed = 0.1f;
    gameOver = false;
    generateFood();
}

void SnakeGame::run() {
    while (window.isOpen()) {
        processEvents();
        if (!gameOver) {
            timer += clock.restart().asSeconds();
            if (timer > speed) {
                timer = 0;
                update();
            }
        }
        render();
    }
}

void SnakeGame::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
        if (event.type == sf::Event::KeyPressed) {
            if (!gameOver) {
                if (event.key.code == sf::Keyboard::Up && dir != Down) dir = Up;
                if (event.key.code == sf::Keyboard::Down && dir != Up) dir = Down;
                if (event.key.code == sf::Keyboard::Left && dir != Right) dir = Left;
                if (event.key.code == sf::Keyboard::Right && dir != Left) dir = Right;
            }
            if (event.key.code == sf::Keyboard::R && gameOver) {
                reset();
            }
        }
    }
}

void SnakeGame::update() {
    move();
    checkCollision();
}

void SnakeGame::move() {
    Segment head = snake.front();
    switch (dir) {
        case Up: head.y -= 1; break;
        case Down: head.y += 1; break;
        case Left: head.x -= 1; break;
        case Right: head.x += 1; break;
    }
    snake.push_front(head);

    if (head.x == food.x && head.y == food.y) {
        eatSound.play();
        score += 10;
        if (speed > 0.03f) speed -= 0.005f;  // Increase speed
        generateFood();
    } else {
        snake.pop_back();
    }
}

void SnakeGame::checkCollision() {
    Segment head = snake.front();
    if (head.x < 0 || head.y < 0 || head.x >= WIDTH / CELL_SIZE || head.y >= HEIGHT / CELL_SIZE) {
        gameOverSound.play();
        gameOver = true;
    }

    for (size_t i = 1; i < snake.size(); ++i) {
        if (snake[i].x == head.x && snake[i].y == head.y) {
            gameOverSound.play();
            gameOver = true;
        }
    }
}

void SnakeGame::generateFood() {
    int maxX = WIDTH / CELL_SIZE;
    int maxY = HEIGHT / CELL_SIZE;
    food.x = rand() % maxX;
    food.y = rand() % maxY;
}

void SnakeGame::render() {
    window.clear();

    // Draw snake
    for (auto& s : snake) {
        sf::RectangleShape rect(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));
        rect.setFillColor(sf::Color::Green);
        rect.setPosition(s.x * CELL_SIZE, s.y * CELL_SIZE);
        window.draw(rect);
    }

    // Draw food
    sf::CircleShape circle(CELL_SIZE / 2 - 1);
    circle.setFillColor(sf::Color::Red);
    circle.setPosition(food.x * CELL_SIZE, food.y * CELL_SIZE);
    window.draw(circle);

    // Draw score
    scoreText.setString("Score: " + std::to_string(score));
    window.draw(scoreText);

    // Draw game over
    if (gameOver) {
        window.draw(gameOverText);
    }

    window.display();
}

int main() {
    srand(static_cast<unsigned>(time(0)));
    SnakeGame game;
    game.run();
    return 0;
}
