#pragma once

#include <cstddef>

#include <SFML/System.hpp>


template <typename T>
struct Vec
{
	Vec() = default;
	Vec(const Vec&) = default;
	Vec(T t) : x(t), y(t) {}
	Vec(T x, T y) : x(x), y(y) {}
	
	template<typename U>
	explicit Vec(sf::Vector2<U> v) : x(v.x), y(v.y) {}

	T x;
	T y;
	
	template <typename U>
	operator Vec<U>() const
	{				
		return Vec<U>{ (U)x, (U)y };
	}

#define VEC_OP(op)						\
	template <typename U>				\
	Vec<T>& operator op(U b)			\
	{									\
		x op b;							\
		y op b;							\
		return *this;					\
	}									\
										\
	template <typename U>				\
	Vec<T>& operator op(Vec<U> b)		\
	{									\
		x op b.x;						\
		y op b.y;						\
		return *this;					\
	}

	VEC_OP(-=)
	VEC_OP(+=)
	VEC_OP(/=)
	VEC_OP(*=)
#undef VEC_OP
};

#define VEC_OP(op)												\
template <typename T, typename U>								\
Vec<decltype(T{} op U{})> operator op(Vec<T> a, Vec<U> b)		\
{																\
	return { a.x op b.x, a.y op b.y };							\
}																\
																\
template <typename T, typename U>								\
Vec<decltype(T{} op U{})> operator op(Vec<T> a, U b)			\
{																\
	return { a.x op b, a.y op b };								\
}																\
																\
template <typename T, typename U>								\
Vec<decltype(T{} op U{}) > operator op(T a, Vec<U> b)			\
{																\
	return { a op b.x, a op b.y };								\
}

VEC_OP(-)
VEC_OP(+)
VEC_OP(/)
VEC_OP(*)
#undef VEC_OP

using Veci9 = Vec<int8_t>;
using Veci16 = Vec<int16_t>;
using Veci32 = Vec<int32_t>;
using Vecf = Vec <float> ;

using Posi = Veci32;
using Posf = Vecf;
using Vel = Vecf;

using Index = int32_t;
constexpr static Index NO_INDEX = (Index)-1;

template <typename Callback>
void makeCircle(Posi radius, Callback&& callback, bool fill = false)
{
	int rx = radius.x;
	int ry = radius.y;

	bool perfectCircle = true;

	if (!rx)
	{
		for (int j = 0; j <= 2 * ry; j++)
		{
			callback({ rx, j });
		}

		return;
	}

	int rxSqr = rx * rx;
	int rySqr = ry * ry;

	int yTop = 0;
	int yBottom;

	int lastTop = -1;

	for (int i = -rx; i <= 0; i++)
	{
		if (perfectCircle)
		{
			//yTop = std::round(std::sqrt((-1 - std::pow(i, 2.0) * std::pow(ry - 0.5, 2.0) + std::pow(rx, 2.0) * std::pow(ry, 2.0)) / std::pow(rx - 0.5, 2.0)) -0.5);

			//yTop = std::sqrt((-1 - std::pow(i, 2.0) * std::pow(ry - 0.5, 2.0) + std::pow(rx, 2.0) * std::pow(ry, 2.0)) / std::pow(rx - 0.5, 2.0));

			//while (std::pow(i, 2.0) * std::pow(ry - 0.5, 2.0) + std::pow(yTop - ry, 2.0) * std::pow(rx - 0.5, 2.0) <= std::pow(rx, 2.0) * std::pow(ry, 2.0))
			{
				//yTop++;
			}

			using std::pow;

			while (pow(i, 2.0) * pow(ry - 0.5, 2.0) + pow(yTop + 1, 2.0) * pow(rx - 0.5, 2.0) <= pow(rx, 2.0) * pow(ry, 2.0))
				yTop++;

		}
		else
		{
			while (std::pow(i, 2.0) * std::pow(ry, 2.0) + std::pow(yTop - ry, 2.0) * std::pow(rx, 2.0) <= std::pow(rx, 2.0) * std::pow(ry, 2.0))
				yTop++;
		}

		if (fill)
		{
			for (int j = -yTop; j <= yTop; j++)
			{
				callback({ -i, j });
				callback({ i, j });
			}
		}
		else
		{
			int target = 0;
			if (yTop == lastTop)
			{
				target = lastTop;
			}
			else 
			{
				target = lastTop + 1;
			}

			for (int j = -yTop; j <= -target; j++)
			{
				//callback({ i, j });
				//callback({ 2 * rx - i, j });
				callback({ -i, -j });
				callback({ -i, j });
				callback({ i, -j });
				callback({ i, j });
			}

		}
		lastTop = yTop;
	}

	//callback({ rx / 2, 0 });
	//callback(rx / 2);
	//bitmap[size.X / 2] = 255;
	//bitmap[size.X * size.Y - size.X / 2 - 1] = 255;
}

template <typename Callback>
void makeCircle(int radius, Callback&& callback, bool fill = false)
{
	makeCircle({ radius, radius }, std::forward< Callback>(callback), fill);
}