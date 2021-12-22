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

enum CellType : Index {Empty = 0, Wall, Sand, Water, Gas, COUNT};
enum ParticleFlags : uint8_t {
	None = 0,
	Movable = 1 << 0,
	Fluid = 1 << 1,
};

struct ParticleDef
{
	float Density = 1;
	uint8_t flags;
};

std::array<ParticleDef, CellType::COUNT> particleDefs = {{
	{0, ParticleFlags::None}, // None
	{1.f, ParticleFlags::None}, // Wall
	{2.f, ParticleFlags::Movable}, // Sand
	{1.5f, ParticleFlags::Movable | ParticleFlags::Fluid}, // Water
	{0.5f, ParticleFlags::Movable | ParticleFlags::Fluid}, // Gas
}};

struct Particle
{
	CellType type;
	Posi pos;
	Posf posf;
	Vel vel;
	Index index;
};

class Simulation
{
public:
	Veci32 worldSize;

	Map<Index> map{ 0, 0 };

	int32_t particleCount = 0;
	std::vector<Particle> particles;

	Simulation(Veci32 worldSize = { 1200, 1000 }) : worldSize(worldSize)
	{
		map.resize(worldSize);
		particles.resize(map.getSize(), {Gas});

		for (int32_t x = 0; x < map.getSize(); x++)
		{
			map.raw(x) = NO_INDEX;
		}


		for (int32_t y = 100; y < worldSize.y / 2; y++)
		{
			for (int32_t x = 0; x  < worldSize.x; x++)
			{
				addParticle(CellType::Gas, x, y);
			}
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

	void addParticle(CellType type, Posi pos)
	{
		Index& index = map.at(pos);
		if (index == NO_INDEX)
		{
			index = particleCount++;
		}
	
		particles[index].pos = pos;
		particles[index].posf = pos;
		particles[index].type = type;
		particles[index].index = index;
	}

	void addParticle(CellType type, int32_t x, int32_t y)
	{
		addParticle(type, {x, y});
	}

	void addParticleCircle(CellType type, Posi center, int32_t radius)
	{
		makeCircle(radius, [&](Posi pos)
		{
			addParticle(type, center + pos);
		});
	}

	void addParticleCircle(CellType type, int32_t x, int32_t y, int32_t radius)
	{
		addParticleCircle(type, { x, y }, radius);
	}

	void fillParticleCircle(CellType type, Posi center, Veci32 radius)
	{
		makeCircle(radius, [&](Posi pos)
		{
			addParticle(type, center + pos);
		}, true);
	}

	CellType getParticleType(Posf pos)
	{
		return getParticleType(Posi(pos + 0.5f));
	}

	CellType getParticleType(Posi pos)
	{
		Index index = map.at(pos);
		return index == NO_INDEX ? CellType::Empty : particles[index].type;
	}

	void moveParticle(Index index, Posf pos)
	{
		Particle& particle = particles[index];
	
		map.at(particle.pos) = NO_INDEX;

		particle.pos = pos + 0.5f;
		particle.posf = pos;

		map.at(particle.pos) = index;
	}

	RandomizerXorshiftPlus randGen;

	void simulate()
	{
		const Vecf gravity { 0.f, 0.01f };

		for (int32_t i = 0; i < particleCount; i++)
		{
			Particle& p = particles[i];
			if ((p.pos.y >= map.getHeight() - 1) || (p.pos.y <= 0) || (p.pos.x >= map.getWidth() - 1) || (p.pos.x <= 0))
			{
				continue;
			}

			const auto& def = particleDefs[p.type];
			if (def.flags & ParticleFlags::Movable)
			{
				const auto gravityForce = gravity * (def.Density - 1);

				p.vel += gravityForce;
				
				float delta = std::max(std::abs(p.vel.x), std::abs(p.vel.y));
				auto step = p.vel / delta;
				auto pos = p.posf;

				while (delta > 0)
				{
					pos += step;
					delta -= 1;

					if (pos.x < 0)
					{
						pos.x = 0;
					}
					else if (pos.x >= map.getWidth())
					{
						pos.x = map.getWidth() - 1;
					}

					if (pos.y < 0)
					{
						pos.y = 0;
					}
					else if (pos.y >= map.getHeight() - 1)
					{
						pos.y = map.getHeight() - 1;
					}

					if (getParticleType(pos) == CellType::Empty)
					{
						moveParticle(i, pos);
					}
					else
					{
						break;
					}
				}
			}
			continue;

			if (p.type == CellType::Sand)
			{
				static const std::array dirsLeft = { Posi{0, 1}, Posi{-1, 1}, Posi{1, 1} };
				static const std::array dirsRight = { Posi{0, 1}, Posi{1, 1}, Posi{-1, 1} };

				const decltype(dirsLeft)& dirs = (randGen.RandomBool() ? dirsLeft : dirsRight);
				for (const Posi& dir : dirs)
				{
					const Posi pos = { p.pos.x + dir.x, p.pos.y + dir.y };
					if (getParticleType(pos) == CellType::Empty)
					{
						moveParticle(i, pos);
						break;
					}
				}
			}
			else if (p.type == CellType::Water)
			{
				static const std::array dirsLeft = { Posi{0, 1}, Posi{-1, 1}, Posi{1, 1}, Posi{-1, 0}, Posi{1, 0} };
				static const std::array dirsRight = { Posi{0, 1}, Posi{1, 1}, Posi{-1, 1}, Posi{1, 0}, Posi{-1, 0} };

				const decltype(dirsLeft)& dirs = (randGen.RandomBool() ? dirsLeft : dirsRight);
				for (const Posi& dir : dirs)
				{
					const Posi pos = { p.pos.x + dir.x, p.pos.y + dir.y };
					if (getParticleType(pos) == CellType::Empty)
					{
						moveParticle(i, pos);
						break;
					}
				}
			}
			else if (p.type == CellType::Gas)
			{
				static const std::array dirsLeft = { Posi{0, -1}, Posi{-1, -1}, Posi{1, -1}, Posi{-1, 0}, Posi{1, 0} };
				static const std::array dirsRight = { Posi{0, -1}, Posi{1, -1}, Posi{-1, -1}, Posi{1, 0}, Posi{-1, 0} };

				const decltype(dirsLeft)& dirs = (randGen.RandomBool() ? dirsLeft : dirsRight);
				for (const Posi& dir : dirs)
				{
					const Posi pos = { p.pos.x + dir.x, p.pos.y + dir.y };
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