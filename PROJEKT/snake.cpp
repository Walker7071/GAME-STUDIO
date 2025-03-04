#include<SFML/Graphics.hpp>
#include<SFML/Network.hpp>
#include<SFML/Audio.hpp>
#include<SFML/Window.hpp>
#include<SFML/System.hpp>
#include <cmath>
#include <iostream>
#include<cstdlib>
using namespace sf;


int N = 35, M =30 ;
int size = 16;
int w = size * N;
int h = size * M;

int dir, num = 4;

struct Snake
{
    int x, y;
}  s[100];

struct Fruit
{
    int x, y;
} f;

void Tick()
{
    for (int i = num; i > 0; --i)
    {
        s[i].x = s[i - 1].x; s[i].y = s[i - 1].y;
    }

    if (dir == 0) s[0].y += 1;
    if (dir == 1) s[0].x -= 1;
    if (dir == 2) s[0].x += 1;
    if (dir == 3) s[0].y -= 1;

    if ((s[0].x == f.x) && (s[0].y == f.y))
    {
        num++; f.x = rand() % N; f.y = rand() % M;
    }

    if (s[0].x > N) s[0].x = 0;  if (s[0].x < 0) s[0].x = N;
    if (s[0].y > M) s[0].y = 0;  if (s[0].y < 0) s[0].y = M;

    for (int i = 1; i < num; i++)
        if (s[0].x == s[i].x && s[0].y == s[i].y)  num = i;
}

constexpr int windowWidth{ 800 }, windowHeight{ 600 };
constexpr float ballRadius{ 10.f }, ballVelocity{ 7.f };
constexpr float paddleWidth{ 100.f }, paddleHeight{ 20.f }, paddleVelocity{ 8.f };
constexpr float blockWidth{ 60.f }, blockHeight{ 20.f };
constexpr int countBlocksX{ 11 }, countBlocksY{ 4 };


struct Ball
{
    sf::CircleShape shape;
    sf::Vector2f velocity{ -ballVelocity, -ballVelocity };

    Ball(float mX, float mY)
    {
        shape.setPosition(mX, mY);
        shape.setRadius(ballRadius);
        //   shape.setFillColor(sf::Color::Black);
        shape.setOrigin(ballRadius, ballRadius);
    }

    void update()
    {
        shape.move(velocity);

        if (left() < 0)
            velocity.x = ballVelocity;
        else if (right() > windowWidth)
            velocity.x = -ballVelocity;

        if (top() < 0)
            velocity.y = ballVelocity;
        else if (bottom() > windowHeight)
        {
            if (shape.getPosition().y > 600)
            {
                exit(0);
            }
            //velocity.y = -ballVelocity;
        }
    }

    float x() { return shape.getPosition().x; }
    float y() { return shape.getPosition().y; }
    float left() { return x() - shape.getRadius(); }
    float right() { return x() + shape.getRadius(); }
    float top() { return y() - shape.getRadius(); }
    float bottom() { return y() + shape.getRadius(); }
};

struct Paddle
{
    sf::RectangleShape shape;
    sf::Vector2f velocity;


    Paddle(float mX, float mY)
    {
        shape.setPosition(mX, mY);
        shape.setSize({ paddleWidth, paddleHeight });


        //shape.setFillColor(sf::Color::Red);
        shape.setOrigin(paddleWidth / 2.f, paddleHeight / 2.f);
    }

    void update()
    {
        shape.move(velocity);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) && left() > 0)
            velocity.x = -paddleVelocity;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) && right() < windowWidth)
            velocity.x = paddleVelocity;
        else
            velocity.x = 0;
    }

    float x() { return shape.getPosition().x; }
    float y() { return shape.getPosition().y; }
    float left() { return x() - shape.getSize().x / 2.f; }
    float right() { return x() + shape.getSize().x / 2.f; }
    float top() { return y() - shape.getSize().y / 2.f; }
    float bottom() { return y() + shape.getSize().y / 2.f; }
};

struct Block
{
    sf::RectangleShape shape;
    bool destroyed{ false };



    Block(float mX, float mY)
    {
        shape.setPosition(mX, mY);
        shape.setSize({ blockWidth, blockHeight });
        shape.setFillColor(sf::Color::Magenta);
        shape.setOrigin(blockWidth / 2.f, blockHeight / 2.f);
    }

    float x() { return shape.getPosition().x; }
    float y() { return shape.getPosition().y; }
    float left() { return x() - shape.getSize().x / 2.f; }
    float right() { return x() + shape.getSize().x / 2.f; }
    float top() { return y() - shape.getSize().y / 2.f; }
    float bottom() { return y() + shape.getSize().y / 2.f; }
};


template <class T1, class T2> bool isIntersecting(T1& mA, T2& mB)
{
    return mA.right() >= mB.left() && mA.left() <= mB.right()
        && mA.bottom() >= mB.top() && mA.top() <= mB.bottom();
}

void testCollision(Paddle& mPaddle, Ball& mBall)
{
    if (!isIntersecting(mPaddle, mBall))
        return;

    mBall.velocity.y = -ballVelocity;

    if (mBall.x() < mPaddle.x())
        mBall.velocity.x = -ballVelocity;
    else
        mBall.velocity.x = ballVelocity;
}

void testCollision(Block& mBlock, Ball& mBall)
{
    if (!isIntersecting(mBlock, mBall))
        return;

    mBlock.destroyed = true;

    float overlapLeft{ mBall.right() - mBlock.left() };
    float overlapRight{ mBlock.right() - mBall.left() };
    float overlapTop{ mBall.bottom() - mBlock.top() };
    float overlapBottom{ mBlock.bottom() - mBall.top() };

    bool ballFromLeft(abs(overlapLeft) < abs(overlapRight));
    bool ballFromTop(abs(overlapTop) < abs(overlapBottom));

    float minOverlapX{ ballFromLeft ? overlapLeft : overlapRight };
    float minOverlapY{ ballFromTop ? overlapTop : overlapBottom };

    if (abs(minOverlapX) < abs(minOverlapY))
        mBall.velocity.x = ballFromLeft ? -ballVelocity : ballVelocity;
    else
        mBall.velocity.y = ballFromTop ? -ballVelocity : ballVelocity;
}

int main()
{
    for (;;) {
        std::cout << "Witaj w salonie gier!" << std::endl;
        std::cout << "Wybierz gre w ktora chcesz zagrac: 1-snake\t2-arkanoid\t3-wyjdz" << std::endl;
        int l;
        std::cin >> l;
        if (l == 1) {
            srand(time(0));

            RenderWindow window(VideoMode(w, h), "Snake");

            Texture t1, t2;
            t1.loadFromFile("white.png");
            t2.loadFromFile("green.png");

            Sprite sprite1(t1);
            Sprite sprite2(t2);

            Clock clock;
            float timer = 0, delay = 0.1;

            f.x = 10;
            f.y = 10;

            while (window.isOpen())
            {
                float time = clock.getElapsedTime().asSeconds();
                clock.restart();
                timer += time;

                Event e;
                while (window.pollEvent(e))
                {
                    if (e.type == Event::Closed)
                        window.close();
                }

                if (Keyboard::isKeyPressed(Keyboard::Left)) dir = 1;
                if (Keyboard::isKeyPressed(Keyboard::Right)) dir = 2;
                if (Keyboard::isKeyPressed(Keyboard::Up)) dir = 3;
                if (Keyboard::isKeyPressed(Keyboard::Down)) dir = 0;

                if (timer > delay) { timer = 0; Tick(); }

                ////// draw  ///////
                window.clear();

                for (int i = 0; i < N; i++)
                    for (int j = 0; j < M; j++)
                    {
                        sprite1.setPosition(i * size, j * size);  window.draw(sprite1);
                    }

                for (int i = 0; i < num; i++)
                {
                    sprite2.setPosition(s[i].x * size, s[i].y * size);  window.draw(sprite2);
                }

                sprite2.setPosition(f.x * size, f.y * size);  window.draw(sprite2);

                window.display();
            }
        }
        else if (l == 2) {
            Ball ball{ windowWidth / 2, windowHeight };
            Paddle paddle{ windowWidth / 2, windowHeight - 50 };
            std::vector<Block> blocks;
            Block block{ windowWidth / 2, windowHeight / 2 };

            sf::Texture texture;
            texture.loadFromFile("bat.jpg");

            paddle.shape.setTexture(&texture);
            paddle.shape.setTextureRect(sf::IntRect(50, 100, 100, 50));

            sf::Texture shu;
            shu.loadFromFile("shu.jpg");

            ball.shape.setTexture(&shu);
            ball.shape.setTextureRect(sf::IntRect(30, 30, 75, 75));

            sf::Texture ceg;
            ceg.loadFromFile("ceg.jpg");

            block.shape.setTexture(&ceg);
            block.shape.setTextureRect(sf::IntRect(30, 30, 75, 75));


            for (int iX{ 0 }; iX < countBlocksX; ++iX)
                for (int iY{ 0 }; iY < countBlocksY; ++iY)
                    blocks.emplace_back((iX + 1) * (blockWidth + 3) + 22,
                        (iY + 2) * (blockHeight + 3));

            sf::RenderWindow window{ {windowWidth, windowHeight}, "Arkanoid" };
            window.setFramerateLimit(60);

            sf::Texture head;
            head.loadFromFile("head.jpg");

            sf::Sprite sprajt;
            sprajt.setTexture(head);

            while (true)
            {
                window.clear(sf::Color::Black);
                sf::Event event;


                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
                    break;
                while (window.pollEvent(event)) {
                    if (event.type == sf::Event::Closed)
                        window.close();
                }
                ball.update();
                paddle.update();

                testCollision(paddle, ball);

                for (auto& block : blocks)
                    testCollision(block, ball);

                blocks.erase(std::remove_if(std::begin(blocks), std::end(blocks),
                    [](const Block& mBlock)
                    {
                        return mBlock.destroyed;
                    }),

                    std::end(blocks));
                sprajt.setScale(1.5, 1.5);
                window.draw(sprajt);

                window.draw(ball.shape);
                window.draw(paddle.shape);
                for (auto& block : blocks)
                    window.draw(block.shape);

                window.display();
            }
        }
        else if (l == 3) {
            exit(0);
        }
        else {
            std::cout << "Wpisz poprawna liczbe!!" << std::endl;
        }
    }
    return 0;
}