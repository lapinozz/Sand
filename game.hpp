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
		simulation({1200, 1000}, 1),
		renderer(simulation)
	{
		const auto worldSize = simulation.worldSize;

		window.setSize(sf::Vector2u(worldSize.x, worldSize.y));
		window.setPosition({ 0, 0 });

		//window.setFramerateLimit(60);

		view = sf::View({ worldSize.x / 2.f, worldSize.y / 2.f }, { (float)worldSize.x, (float)worldSize.y });
		window.setView(view);

		font.loadFromFile("arial.ttf");

		fpsText = sf::Text("", font, 10);
		frameTimeText = sf::Text("", font, 10);

		fpsText.setPosition({ 10.f, 10.f });
		frameTimeText.setPosition({ 10.f, 30.f });
	}

	void run()
	{
		const auto worldSize = simulation.worldSize;
		const auto worldRes = simulation.worldRes;

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
				}
				else if (event.type == sf::Event::MouseButtonReleased)
				{
					mouseDown = false;
				}
				else if (event.type == sf::Event::MouseMoved)
				{
					if (mouseDown)
					{
						auto pos = window.mapPixelToCoords({ event.mouseMove.x, event.mouseMove.y }, view);
						pos.x /= worldRes;
						pos.y /= worldRes;
						//addParticle(mouseCellType, pos.x, pos.y);
						simulation.fillParticleCircle(mouseCellType, (int32_t)pos.x, (int32_t)pos.y, 5);
					}
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

			//simulation.simulate();

			frameCounter++;
			fpsTime += frameClock.restart().asSeconds();
			if (fpsTime >= 10.f)
			{
				fpsTime -= 10.f;

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
				continue;
			}

			window.clear();

			window.draw(renderer);

			window.draw(fpsText);
			window.draw(frameTimeText);

			window.display();
		}
	}
};