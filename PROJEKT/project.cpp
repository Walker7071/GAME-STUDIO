#include<iostream>
#include<SFML\Graphics.hpp>
#include<SFML\Audio.hpp>
#include<SFML\Network.hpp>
#include<SFML\Main.hpp>
#include<ctime>

//#include"Menu.cpp"
using namespace sf;
//using namespace std;

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


int N = 35, M = 30;
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

int main()
{
	int w, e;
	std::cout << "Welcome to MENU" << std::endl << "1.Arkanoid" << std::endl << "2.Snake" << std::endl << "3.Exit" << std::endl;
	std::cin >> w;
	for (;;) {
		if (w == 1) {
			std::cout << "Which difficulty level do you want to choose?:\t1-Easy\t2-Hard" << std::endl;
			for (;;) {
				std::cin >> e;
				if (e == 2) {
					float a = 1, b = 1;

					RenderWindow window(VideoMode(800, 600), "Arkanoid");
					window.setFramerateLimit(60);

					//Menu menu(window.getSize().x, window.getSize().y);

					Texture t1, t2, t3, t4;
					t1.loadFromFile("block1.jpg");
					t2.loadFromFile("background.jpg");
					t3.loadFromFile("ball.png");
					t4.loadFromFile("paddle.jpg");

					Sprite background(t2), ball(t3), paddle(t4);
					paddle.setPosition(350, 570);
					ball.setPosition(394, 558);

					Sprite block[1000];

					int n = 0;
					for (int i = 1; i <= 10; i++)
						for (int j = 1; j <= 10; j++)
						{
							block[n].setTexture(t1);
							block[n].setPosition((i - 1) * 80, j * 20);
							n++;
						}
					float dx = 7, dy = 6;
					float x = 300, y = 300;
					int p = 0;
					while (window.isOpen())
					{
						Event event, ev;
						while (window.pollEvent(event))
						{
							if (event.type == Event::Closed)
								window.close();
						}
						x += dx;
						for (int i = 0; i < n; i++)
							if (FloatRect(x + 3, y + 3, 6, 6).intersects(block[i].getGlobalBounds()))
							{
								block[i].setPosition(-100, 0); dx = -dx;
								p++;
							}

						y += dy;
						for (int i = 0; i < n; i++)
							if (FloatRect(x + 3, y + 3, 6, 6).intersects(block[i].getGlobalBounds()))
							{
								block[i].setPosition(-100, 0); dy = -dy;
								p++;
							}

						if (x < 0 || x>800)  dx = -dx;
						if (y < 0 || y>600)  dy = -dy;

						if (Keyboard::isKeyPressed(Keyboard::Right)) paddle.move(8, 0);
						if (Keyboard::isKeyPressed(Keyboard::Left)) paddle.move(-8, 0);
						if (Keyboard::isKeyPressed(Keyboard::Up)) paddle.move(0, -8);
						if (Keyboard::isKeyPressed(Keyboard::Down)) paddle.move(0, 8);
						//if (Keyboard::isKeyPressed(Keyboard::Space)) ball.setScale(a++,b++);


						if (FloatRect(x, y, 24, 24).intersects(paddle.getGlobalBounds())) dy = -(rand() % 5 + 2);

						if (y > 600)
						{
							window.close();
							
							std::cout << "broken bricks: " << p << std::endl;

						}



						ball.setPosition(x, y);
						window.clear();
						//menu.draw(window);

						window.draw(background);
						window.draw(ball);

						window.draw(paddle);

						for (int i = 0; i < n; i++)
							window.draw(block[i]);

						window.display();
					}

				}
				else if (e == 1) {
					Ball ball{ windowWidth / 2, windowHeight };
					Paddle paddle{ windowWidth / 2, windowHeight - 50 };
					std::vector<Block> blocks;
					Block block{ windowWidth / 2, windowHeight / 2 };

					//sf::Texture texture;
					//texture.loadFromFile("bat.jpg");

					//paddle.shape.setTexture(&texture);
					//paddle.shape.setTextureRect(sf::IntRect(50, 100, 100, 50));

					sf::Texture shu;
					shu.loadFromFile("shu.jpg");

					ball.shape.setTexture(&shu);
					ball.shape.setTextureRect(sf::IntRect(30, 30, 75, 75));

					//sf::Texture ceg;
					//ceg.loadFromFile("ceg.jpg");

					//block.shape.setTexture(&ceg);
					//block.shape.setTextureRect(sf::IntRect(30, 30, 75, 75));


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
				else
					std::cout << "Please select a valid mode" << std::endl;
			}

		}
		else if (w == 2)
		{
			srand(time(0));
			int n = 0;
			RenderWindow window(VideoMode(560, 480), "Snake - press X to leave");

			Texture t1, t2, t3;
			t1.loadFromFile("white.png");
			t2.loadFromFile("green.png");
			t3.loadFromFile("apple.png");
			Sprite sprite1(t1);
			Sprite sprite2(t2);
			Sprite sprite3(t3);
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
				if (Keyboard::isKeyPressed(Keyboard::X)) return 0;

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

				sprite3.setPosition(f.x * size, f.y * size);  window.draw(sprite3);

				window.display();

			}
		}
		else if (w == 3)
		{
			exit(0);
		}
		else
		{
			std::cout << "Invalid selection, please try again." << std::endl;
		}
	}
	return 0;
}