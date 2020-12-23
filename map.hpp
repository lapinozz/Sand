#pragma once

#include <cstddef>
#include <vector>

#include "utils.hpp"

template <typename T>
class Map
{
	using IndexType = int32_t;

	IndexType width;
	IndexType height;

	std::vector<T> data;

public:
	Map(IndexType width, IndexType height) : width(width), height(height)
	{
		data.resize((size_t)width * (size_t)height);
	}

	const T& at(IndexType x, IndexType y) const
	{
		return data.data()[y * width + x];
	}

	T& at(IndexType x, IndexType y)
	{
		return data.data()[y * width + x];
	}

	const T& at(Pos pos) const
	{
		return at(pos.x, pos.y);
	}

	T& at(Pos pos)
	{
		return at(pos.x, pos.y);;
	}

	const T& raw(IndexType index) const
	{
		return data.data()[index];
	}

	T& raw(IndexType index)
	{
		return data.data()[index];
	}

	IndexType getWidth() const
	{
		return width;
	}

	IndexType getHeight() const
	{
		return height;
	}

	IndexType getSize() const
	{
		return width * height;
	}

	void resize(Vec size)
	{
		width = size.x;
		height = size.x;
		data.resize((size_t)width * (size_t)height);
	}
};