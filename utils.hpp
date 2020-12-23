#pragma once

#include <cstddef>

#include <SFML/System.hpp>

struct Pos
{
	int32_t x;
	int32_t y;
};

using Vec = Pos;

using Index = int32_t;
constexpr static Index NO_INDEX = (Index)-1;