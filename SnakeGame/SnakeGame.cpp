#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <SFML/Audio.hpp>

int main() {
	srand(static_cast<unsigned>(time(0)));

	const int windowWidth = 960;
	const int windowHeight = 640;
	const int cellSize = 32;
	int score = 0;
	bool isGameOver = false;
	bool directionChanged = false;

	sf::Texture headTexture, bodyTexture, foodTexture, backgroundTexture, head2Texture;
	sf::Texture wall1Texture, wallc1Texture, wallc11Texture, wall2Texture, wallc2Texture, wallc22Texture;

	if (!headTexture.loadFromFile("assets/head.png"))
		std::cerr << "Failed to load head.png\n";

	if (!bodyTexture.loadFromFile("assets/body.png"))
		std::cerr << "Failed to load body.png\n";

	if (!foodTexture.loadFromFile("assets/food.png"))
		std::cerr << "Failed to load food.png\n";
	if (!backgroundTexture.loadFromFile("assets/background.png")) {
		std::cerr << "Failed to load background.png\n";
	}
	if (!head2Texture.loadFromFile("assets/head2.png")) {
		std::cerr << "Failed to load background.png\n";
	}

	if (!wall1Texture.loadFromFile("assets/wall1.png")) {
		std::cerr << "Failed to load background.png\n";
	}
	if (!wallc1Texture.loadFromFile("assets/wallc1.png")) {
		std::cerr << "Failed to load background.png\n";
	}
	if (!wallc11Texture.loadFromFile("assets/wallc11.png")) {
		std::cerr << "Failed to load background.png\n";
	}
	if (!wall2Texture.loadFromFile("assets/wall2.png")) {
		std::cerr << "Failed to load background.png\n";
	}
	if (!wallc2Texture.loadFromFile("assets/wallc2.png")) {
		std::cerr << "Failed to load background.png\n";
	}
	if (!wallc22Texture.loadFromFile("assets/wallc22.png")) {
		std::cerr << "Failed to load background.png\n";
	}

	sf::Sprite headSprite(headTexture);
	sf::Sprite bodySprite(bodyTexture);
	sf::Sprite foodSprite(foodTexture);
	sf::Sprite backgroundSprite(backgroundTexture);
	sf::Sprite head2Sprite(head2Texture);

	//sound
	sf::SoundBuffer eatBuffer;
	if (!eatBuffer.loadFromFile("assets/eat.wav")) {
		std::cerr << "Failed to load eat.wav\n";
		return -1;
	}

	sf::SoundBuffer gameOverBuffer;
	if (!gameOverBuffer.loadFromFile("assets/gameover.wav")) {
		std::cerr << "Failed to load gameover.wav\n";
	}

	sf::Sound eatSound(eatBuffer);
	sf::Sound gameOverSound(gameOverBuffer);

	sf::Music bgMusic;
	if (!bgMusic.openFromFile("assets/bgm.wav")) {
		std::cerr << "Failed to load background.ogg\n";
	}
	bgMusic.setLooping(true);
	bgMusic.setVolume(50);
	bgMusic.play();

	sf::RenderWindow window(sf::VideoMode({ windowWidth, windowHeight }), "Snake!");

	const sf::Font font("assets/arial.ttf");

	//score
	sf::Text scoreText(font, "Score: 0");
	scoreText.setCharacterSize(32);
	scoreText.setFillColor(sf::Color::White);
	scoreText.setPosition({ 10.f, 10.f });

	//gameover text
	sf::Text gameOverText(font, "");
	gameOverText.setCharacterSize(40);
	gameOverText.setFillColor(sf::Color::Red);

	std::vector<sf::RectangleShape> walls;
	sf::Color wallColor = sf::Color(100, 100, 100);

	// Tường trên và dưới
	for (int x = cellSize; x < windowWidth - cellSize; x += cellSize) {
		sf::RectangleShape wallTop({ float(cellSize), float(cellSize) });
		wallTop.setPosition({ float(x), float(2 * cellSize) });
		if (x == cellSize) {
			wallTop.setTexture(&wallc1Texture);
		}
		else if (x == windowWidth - 2 * cellSize) {
			wallTop.setTexture(&wallc11Texture);
		}
		else {
			wallTop.setTexture(&wall1Texture);
		}
		walls.push_back(wallTop);

		sf::RectangleShape wallBottom({ float(cellSize), float(cellSize) });
		wallBottom.setPosition({ float(x), float(windowHeight - cellSize) });
		wallBottom.setTexture(&wall1Texture);
		walls.push_back(wallBottom);
	}

	// Tường trái và phải
	for (int y = 0; y < windowHeight - 2 * cellSize; y += cellSize) {
		sf::RectangleShape wallLeft({ float(cellSize), float(cellSize) });
		wallLeft.setPosition({ 0.f , float(y + 2 * cellSize) });
		if (y == 0) {
			wallLeft.setTexture(&wallc22Texture);
		}
		else if (y == windowHeight - 3 * cellSize) {
			wallLeft.setTexture(&wallc2Texture);
		}
		else {
			wallLeft.setTexture(&wall2Texture);
		}
		walls.push_back(wallLeft);

		sf::RectangleShape wallRight({ float(cellSize), float(cellSize) });
		wallRight.setPosition({ float(windowWidth - cellSize), float(y + 2 * cellSize) });
		if (y == 0) {
			wallRight.setTexture(&wallc22Texture);
		}
		else if (y == windowHeight - 3 * cellSize) {
			wallRight.setTexture(&wallc2Texture);
		}
		else {
			wallRight.setTexture(&wall2Texture);
		}
		walls.push_back(wallRight);
	}

	// Rắn ban đầu
	std::vector<sf::RectangleShape> snake;

	sf::Vector2f headPos = { 320.f, 320.f };
	sf::Vector2f tailPos = { 288.f, 320.f };

	//snake head
	sf::RectangleShape head({ float(cellSize), float(cellSize) });
	head.setPosition(headPos);
	snake.push_back(head);

	//snake body
	sf::RectangleShape tail({ float(cellSize), float(cellSize) });
	tail.setPosition(tailPos);
	snake.push_back(tail);

	//food
	sf::RectangleShape food({ float(cellSize), float(cellSize) });
	food.setFillColor(sf::Color::Red);

	sf::Vector2f direction = { float(cellSize), 0.f };
	sf::Vector2f newDirection = direction;

	sf::Clock clock;
	float moveDelay = 0.1f;

	auto spawnFood = [&food, &foodSprite, &snake, windowWidth, windowHeight, cellSize]() {
		while (true) {
			int maxX = (windowWidth - cellSize * 2) / cellSize;
			int maxY = (windowHeight - cellSize * 2) / cellSize;
			int x = (rand() % maxX + 1) * cellSize;
			int y = (rand() % (maxY - 2) + 3) * cellSize;

			sf::Vector2f newFoodPos = { float(x), float(y) };
			sf::FloatRect newFoodBounds(newFoodPos, { float(cellSize), float(cellSize) });

			bool overlapsSnake = false;
			for (const auto& part : snake) {
				if (part.getGlobalBounds().findIntersection(newFoodBounds)) {
					overlapsSnake = true;
					break;
				}
			}

			if (!overlapsSnake) {
				food.setPosition(newFoodPos);
				foodSprite.setPosition(newFoodPos);

				break;
			}
		}
		};

	spawnFood();
	bool hasPlayedGameOverSound = false;

	while (window.isOpen()) {
		while (const std::optional event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>())
				window.close();
		}

		if (isGameOver) {
			if (!hasPlayedGameOverSound) {
				gameOverSound.play();
				sf::sleep(sf::seconds(1)); // Đợi một chút để âm thanh kết thúc
				hasPlayedGameOverSound = true;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R)) {
				snake.clear();
				sf::RectangleShape head({ float(cellSize), float(cellSize) });
				head.setPosition({ 320.f, 320.f });
				snake.push_back(head);
				sf::RectangleShape tail({ float(cellSize), float(cellSize) });
				tail.setPosition({ 288.f, 320.f });
				snake.push_back(tail);
				direction = { float(cellSize), 0.f };
				newDirection = direction;
				spawnFood();
				score = 0;
				scoreText.setString("Score: 0");
				isGameOver = false;
				hasPlayedGameOverSound = false;

				clock.restart();
			}
		}

		if (!directionChanged) {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
				newDirection = { 0.f, -float(cellSize) };
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
				newDirection = { 0.f, float(cellSize) };
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
				newDirection = { -float(cellSize), 0.f };
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
				newDirection = { float(cellSize), 0.f };
			}

			if ((newDirection.x + direction.x != 0 || newDirection.y + direction.y != 0) &&
				(newDirection.x != direction.x || newDirection.y != direction.y)) {
				direction = newDirection;
				directionChanged = true;
			}
		}

		if (clock.getElapsedTime().asSeconds() >= moveDelay) {
			sf::Vector2f newHeadPos = snake[0].getPosition() + direction;

			sf::RectangleShape newHead({ float(cellSize), float(cellSize) });
			newHead.setFillColor(sf::Color::Green);
			newHead.setPosition(newHeadPos);

			for (std::size_t i = 0; i < snake.size(); ++i) {
				if (newHead.getGlobalBounds().findIntersection(snake[i].getGlobalBounds())) {
					isGameOver = true;
					gameOverText.setString("     Game Over\n      Score: " + std::to_string(score) + "\nPress R to Restart");
					gameOverText.setPosition({ windowWidth / 2.f - 150.f, windowHeight / 2.f - 100 });
					break;
				}
			}

			if (newHeadPos.x < cellSize || newHeadPos.y < 3 * cellSize ||
				newHeadPos.x + cellSize > windowWidth - cellSize ||
				newHeadPos.y + cellSize > windowHeight - cellSize) {
				isGameOver = true;

				gameOverText.setString("     Game Over\n      Score: " + std::to_string(score) + "\nPress R to Restart");
				gameOverText.setPosition({ windowWidth / 2.f - 150.f, windowHeight / 2.f - 100.f });
			}

			if (!isGameOver) {
				if (newHead.getGlobalBounds().findIntersection(food.getGlobalBounds())) {
					snake.insert(snake.begin(), newHead);
					spawnFood();
					eatSound.play();
					score += 10;
					scoreText.setString("Score: " + std::to_string(score));
				}
				else {
					snake.insert(snake.begin(), newHead);
					snake.pop_back();
				}
				clock.restart();
				directionChanged = false;
			}
		}

		window.clear();
		window.draw(backgroundSprite);
		window.draw(foodSprite);
		for (auto& wallPart : walls) window.draw(wallPart);

		for (std::size_t i = 0; i < snake.size(); ++i) {
			sf::Vector2f pos = snake[i].getPosition();

			if (i == 0) {
				headSprite.setOrigin({ headTexture.getSize().x / 2.f, headTexture.getSize().y / 2.f });
				headSprite.setPosition({ pos.x + cellSize / 2.f, pos.y + cellSize / 2.f });

				head2Sprite.setOrigin({ headTexture.getSize().x / 2.f, headTexture.getSize().y / 2.f });
				head2Sprite.setPosition({ pos.x + cellSize / 2.f, pos.y + cellSize / 2.f });


				// Xoay đầu rắn tùy hướng
				if (!isGameOver) {
					if (direction.x > 0) {
						headSprite.setRotation(sf::degrees(270.f));     // phải
						head2Sprite.setRotation(sf::degrees(270.f));
					}
					else if (direction.x < 0) {
						headSprite.setRotation(sf::degrees(90.f));
						head2Sprite.setRotation(sf::degrees(90.f));
					}
					else if (direction.y < 0) {
						headSprite.setRotation(sf::degrees(180.f));
						head2Sprite.setRotation(sf::degrees(180.f));
					}
					else if (direction.y > 0) {
						headSprite.setRotation(sf::degrees(0.f));
						head2Sprite.setRotation(sf::degrees(0.f));
					}

				}

				if (!isGameOver)
					window.draw(headSprite);
				else
					window.draw(head2Sprite);
			}
			else {
				bodySprite.setPosition(pos);
				window.draw(bodySprite);
			}
		}
		if (!isGameOver)
			window.draw(scoreText);
		if (isGameOver)
			window.draw(gameOverText);
		window.display();
	}

	return 0;
}
