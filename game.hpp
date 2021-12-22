// Sand.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <array>
#include <vector>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <sstream>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "random.hpp"
#include "Simulation.hpp"
#include "SimulationRenderer.hpp"

#pragma comment(lib, "opengl32.lib")

class Game
{
public:

	sf::ContextSettings contextSettings;
	sf::RenderWindow window;

	int worldRes = 2;
	Veci32 worldSize = {700, 500};

	Simulation simulation;
	SimulationRenderer renderer;

	sf::View view;

	bool mouseDown = false;
	CellType mouseCellType = CellType::Wall;

	sf::Font font;

	sf::Text fpsText;
	sf::Text frameTimeText;

	int32_t frameCounter = 0;
	sf::Clock frameClock;
	double fpsTime = 0.f;

	float displayTime = 0.f;
	sf::Clock displayClock;

	Game() : contextSettings(
			24, // settings.depthBits
			8, // settings.stencilBits
			4, // settings.antialiasingLevel
			3, // settings.majorVersion
			0 // settings.minorVersion
		),

		window(sf::VideoMode(), "Sand", sf::Style::Close | sf::Style::Resize, contextSettings), 
		simulation(worldSize),
		renderer(simulation, worldRes)
	{
		const auto viewSize = worldSize * worldRes;

		window.setSize(sf::Vector2u(worldSize.x * worldRes, worldSize.y * worldRes));
		window.setPosition({ 0, 0 });

		//window.setFramerateLimit(60);

		view = sf::View({ viewSize.x / 2.f, viewSize.y / 2.f }, { (float)viewSize.x, (float)viewSize.y });
		window.setView(view);

		font.loadFromFile("arial.ttf");

		fpsText = sf::Text("", font, 10);
		frameTimeText = sf::Text("", font, 10);

		fpsText.setPosition({ 10.f, 10.f });
		frameTimeText.setPosition({ 10.f, 30.f });
	}

	void run()
	{
		Veci32 radius{ 5, 5 };
		sf::Image brushImg;
		sf::Texture brushTexture;
		sf::Sprite brushSprite(brushTexture);

		const auto updateBrush = [&]()
		{
			const auto size = radius * 2 + 3;
			brushImg.create(size.x, size.y, sf::Color::Transparent);

			const auto center = radius + 1;

			makeCircle(radius, [&](Posi pos)
			{
				brushImg.setPixel(pos.x + center.x, pos.y + center.y, sf::Color::White);
			});

			brushTexture.create(size.x, size.y);
			brushTexture.update(brushImg);

			brushSprite.setTexture(brushTexture, true);
			brushSprite.setOrigin(center.x, center.y);
			brushSprite.setScale(worldRes, worldRes);
		};

		const auto draw = [&](Posi pos)
		{
			//addParticle(mouseCellType, pos.x, pos.y);
			simulation.fillParticleCircle(mouseCellType, pos, radius);
			//simulation.addParticleCircle(mouseCellType, (int32_t)pos.x, (int32_t)pos.y, 5);
		};

		updateBrush();

		while (window.isOpen())
		{
			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
				{
					window.close();
				}
				else if (event.type == sf::Event::MouseButtonPressed)
				{
					mouseDown = true;

					Veci32 pos{ window.mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y }, view) };
					pos /= worldRes;
					draw(Veci32{ pos });
				}
				else if (event.type == sf::Event::MouseButtonReleased)
				{
					mouseDown = false;
				}
				else if (event.type == sf::Event::MouseMoved)
				{
					Veci32 pos{ window.mapPixelToCoords({ event.mouseMove.x, event.mouseMove.y }, view) };

					brushSprite.setPosition(pos.x, pos.y);

					if (mouseDown)
					{
						pos /= worldRes;
						draw(Veci32{ pos });
					}
				}
				else if (event.type == sf::Event::MouseWheelScrolled)
				{
					if (event.mouseWheelScroll.delta < 0)
					{
						radius.y--;

						if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
						{
							radius.x--;
						}
					}
					else
					{
						radius.y++;

						if(!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
						{
							radius.x++;
						}
					}
					
					updateBrush();
				}
				if (event.type == sf::Event::KeyPressed)
				{
					if (event.key.code == sf::Keyboard::Num1)
					{
						mouseCellType = CellType::Empty;
					}
					else if (event.key.code == sf::Keyboard::Num2)
					{
						mouseCellType = CellType::Wall;
					}
					else if (event.key.code == sf::Keyboard::Num3)
					{
						mouseCellType = CellType::Sand;
					}
					else if (event.key.code == sf::Keyboard::Num4)
					{
						mouseCellType = CellType::Water;
					}
					else if (event.key.code == sf::Keyboard::Num5)
					{
						mouseCellType = CellType::Gas;
					}
				}
			}

			simulation.simulate();

			frameCounter++;
			fpsTime += frameClock.restart().asSeconds();
			if (fpsTime >= 1.f)
			{
				fpsTime -= 1.f;

				std::cout << frameCounter << std::endl;

				fpsText.setString(std::to_string(frameCounter));
				frameTimeText.setString(std::to_string((1.f / frameCounter) * 1000));

				frameCounter = 0;
			}

			displayTime += displayClock.restart().asSeconds();
			if (displayTime >= 1.f / 60.f)
			{
				displayTime -= (1.f / 60.f);
			}
			else
			{
				//continue;
			}

			window.clear();

			window.draw(renderer);

			window.draw(brushSprite);

			window.draw(fpsText);
			window.draw(frameTimeText);

			window.display();
		}
	}
};