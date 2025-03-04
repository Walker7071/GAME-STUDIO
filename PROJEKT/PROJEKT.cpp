#include<SFML/Graphics.hpp>
#include<SFML/Network.hpp>
#include<SFML/Audio.hpp>
#include<SFML/Window.hpp>
#include<SFML/System.hpp>
#include <cmath>
#include <iostream>
#include<cstdlib>

constexpr int windowWidth{ 800 }, windowHeight{ 600 };
constexpr float ballRadius{ 10.f }, ballVelocity{ 7.f };
constexpr float paddleWidth{ 500.f }, paddleHeight{ 20.f }, paddleVelocity{ 7.f };
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

    return 0;
}