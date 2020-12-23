#pragma once

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
#include "utils.hpp"
#include "map.hpp"

enum CellType : Index {Empty = 0, Wall, Sand, Water, Gas};

struct Color
{
	Color(const sf::Color& c)
	{
		r = c.r / 255.f;
		g = c.g / 255.f;
		b = c.b / 255.f;
		a = c.a / 255.f;
	}

	float r;
	float g;
	float b;
	float a;
};
std::array cellColors2 = { sf::Color::Transparent, sf::Color{128, 0, 0}, sf::Color{194, 178, 128}, sf::Color{156, 211, 219}, sf::Color{76, 187, 23} };
std::array<Color, 5> cellColors = { sf::Color::Transparent, sf::Color{128, 0, 0}, sf::Color{194, 178, 128}, sf::Color{156, 211, 219}, sf::Color{76, 187, 23} };


struct Particle
{
	CellType type;
	Pos pos;
	Index index;
};

class Simulation
{
public:
	Vec worldSize;
	int32_t worldRes;

	Map<Index> map{ 0, 0 };

	int32_t particleCount = 0;
	std::vector<Particle> particles;

	Simulation(Vec worldSize = { 1200, 1000 }, int32_t worldRes = 4) : worldSize(worldSize), worldRes(worldRes)
	{
		map.resize(worldSize);
		particles.resize(map.getSize(), {Gas});

		for (int32_t x = 0; x < map.getSize(); x++)
		{
			map.raw(x) = NO_INDEX;
		}

		for (int32_t x = 0; x < 50; x++)
		{
			addParticle(CellType::Wall, 125 + x, 150);
		}

		for (int32_t x = 0; x < 50; x++)
		{
			addParticle(CellType::Wall, 125 + x, 210);
		}

		for (int32_t x = 0; x < 25; x++)
		{
			addParticle(CellType::Wall, 125, 210 - x);
			addParticle(CellType::Wall, 175, 210 - x);
		}

		for (int32_t y = 50; y < 100; y++)
		{
			for (int32_t x = 150 + (75 - y); x < 150 - (75 - y); x++)
			{
				addParticle(CellType::Sand, x, y);
			}
		}

		for (int32_t y = 185; y < 210; y++)
		{
			for (int32_t x = 150 - (197 - y); x < 150 + (197 - y); x++)
			{
				addParticle(CellType::Water, x, y);
			}
		}
	}

	void addParticle(CellType type, Pos pos)
	{
		Index& index = map.at(pos);
		if (index == NO_INDEX)
		{
			index = particleCount++;
		}
	
		particles[index].pos = pos;
		particles[index].type = type;
		particles[index].index = index;
	}

	void addParticle(CellType type, int32_t x, int32_t y)
	{
		addParticle(type, {x, y});
	}

	void addParticleCircle(CellType type, Pos center, int32_t radius)
	{
		Pos pos{ radius, 0 };
	
		addParticle(type, { center.x + pos.x, center.y + pos.y });

		if (radius > 0)
		{
			addParticle(type, { center.x + pos.x, center.y - pos.y });
			addParticle(type, { center.x + pos.y, center.y + pos.x });
			addParticle(type, { center.x - pos.y, center.y + pos.x });
			addParticle(type, { center.x - pos.y, center.y + pos.x });
		}

		auto p = 1 - radius;
		while (pos.x > pos.y)
		{
			pos.y++;

			if (p <= 0)
			{
				p = p + 2 * pos.y + 1;
			}
			else
			{
				pos.x--;
				p = p + 2 * pos.y - 2 * pos.x + 1;
			}

			if (pos.x < pos.y)
			{
				break;
			}

			addParticle(type, { center.x + pos.x, center.y + pos.y });
			addParticle(type, { center.x - pos.x, center.y + pos.y });
			addParticle(type, { center.x - pos.x, center.y - pos.y });
			addParticle(type, {center.x + pos.x, center.y - pos.y});

			if (pos.x != pos.y)
			{
				addParticle(type, { center.x + pos.y, center.y + pos.x });
				addParticle(type, { center.x - pos.y, center.y + pos.x });
				addParticle(type, { center.x - pos.y, center.y - pos.x });
				addParticle(type, { center.x + pos.y, center.y - pos.x });
			}
		}
	}

	void addParticleCircle(CellType type, int32_t x, int32_t y, int32_t radius)
	{
		addParticleCircle(type, { x, y }, radius);
	}

	void fillParticleCircle(CellType type, Pos center, int32_t radius)
	{
		Pos pos{ radius, 0 };

		if (radius == 0)
		{
			addParticle(type, { center.x + pos.x, center.y + pos.y });
		}
		else
		{
			for (int32_t i = -radius; i < radius + 1; i++)
			{
				addParticle(type, { center.x + i, center.y});
			}

			addParticle(type, { center.x + pos.y, center.y + pos.x });
			addParticle(type, { center.x + pos.y, center.y - pos.x });
		}

		auto p = 1 - radius;
		while (pos.x > pos.y)
		{
			pos.y++;

			if (p <= 0)
			{
				p = p + 2 * pos.y + 1;
			}
			else
			{
				pos.x--;
				p = p + 2 * pos.y - 2 * pos.x + 1;
			}

			if (pos.x < pos.y)
			{
				break;
			}

			for (int32_t i = 0; i < std::abs(pos.x) + 1; i++)
			{
				addParticle(type, { center.x + i, center.y + pos.y });
				addParticle(type, { center.x - i, center.y + pos.y });
				addParticle(type, { center.x - i, center.y - pos.y });
				addParticle(type, { center.x + i, center.y - pos.y });
			}

			if (pos.x != pos.y)
			{
				for (int32_t i = 0; i < std::abs(pos.x) + 1; i++)
				{
					addParticle(type, { center.x + pos.y, center.y + i });
					addParticle(type, { center.x - pos.y, center.y + i });
					addParticle(type, { center.x - pos.y, center.y - i });
					addParticle(type, { center.x + pos.y, center.y - i });
				}
			}
		}
	}

	void fillParticleCircle(CellType type, int32_t x, int32_t y, int32_t radius)
	{
		fillParticleCircle(type, { x, y }, radius);
	}

	CellType getParticleType(Pos pos)
	{
		Index index = map.at(pos);
		return index == NO_INDEX ? CellType::Empty : particles[index].type;
	}

	void moveParticle(Index index, Pos pos)
	{
		Particle& particle = particles[index];
	
		map.at(particle.pos) = NO_INDEX;
		map.at(pos) = index;

		particle.pos = pos;
	}

	RandomizerXorshiftPlus randGen;

	void simulate()
	{
		for (int32_t i = 0; i < particleCount; i++)
		{
			Particle& p = particles[i];
			if ((p.pos.y >= map.getHeight() - 1) || (p.pos.y <= 0) || (p.pos.x >= map.getWidth() - 1) || (p.pos.x <= 0))
			{
				continue;
			}

			if (p.type == CellType::Sand)
			{
				static const std::array dirsLeft = { Pos{0, 1}, Pos{-1, 1}, Pos{1, 1} };
				static const std::array dirsRight = { Pos{0, 1}, Pos{1, 1}, Pos{-1, 1} };

				const decltype(dirsLeft)& dirs = (randGen.RandomBool() ? dirsLeft : dirsRight);
				for (const Pos& dir : dirs)
				{
					const Pos pos = { p.pos.x + dir.x, p.pos.y + dir.y };
					if (getParticleType(pos) == CellType::Empty)
					{
						moveParticle(i, pos);
						break;
					}
				}
			}
			else if (p.type == CellType::Water)
			{
				static const std::array dirsLeft = { Pos{0, 1}, Pos{-1, 1}, Pos{1, 1}, Pos{-1, 0}, Pos{1, 0} };
				static const std::array dirsRight = { Pos{0, 1}, Pos{1, 1}, Pos{-1, 1}, Pos{1, 0}, Pos{-1, 0} };

				const decltype(dirsLeft)& dirs = (randGen.RandomBool() ? dirsLeft : dirsRight);
				for (const Pos& dir : dirs)
				{
					const Pos pos = { p.pos.x + dir.x, p.pos.y + dir.y };
					if (getParticleType(pos) == CellType::Empty)
					{
						moveParticle(i, pos);
						break;
					}
				}
			}
			else if (p.type == CellType::Gas)
			{
				static const std::array dirsLeft = { Pos{0, -1}, Pos{-1, -1}, Pos{1, -1}, Pos{-1, 0}, Pos{1, 0} };
				static const std::array dirsRight = { Pos{0, -1}, Pos{1, -1}, Pos{-1, -1}, Pos{1, 0}, Pos{-1, 0} };

				const decltype(dirsLeft)& dirs = (randGen.RandomBool() ? dirsLeft : dirsRight);
				for (const Pos& dir : dirs)
				{
					const Pos pos = { p.pos.x + dir.x, p.pos.y + dir.y };
					if (getParticleType(pos) == CellType::Empty)
					{
						moveParticle(i, pos);
						break;
					}
				}
			}
		}
	}
};