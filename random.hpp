#pragma once

#include <random>

class RandomizerXorshiftPlus
{
public:
	RandomizerXorshiftPlus()
		: m_rand(0),
		counter(0),
		randomizer(0, std::numeric_limits<uint64_t>::max())
	{
		// Seed xorshift128plus taking into account that random_device may return 32-bit values
		s[0] = (uint64_t(rd()) << 32) | (rd() & 0xffffffff);
		s[1] = (uint64_t(rd()) << 32) | (rd() & 0xffffffff);
	}

	bool RandomBool()
	{
		if (!counter)
		{
			m_rand = randomizer(gen);
			counter = sizeof(uint64_t) * CHAR_BIT;
		}
		return (m_rand >> --counter) & 1;
	}
private:
	uint64_t s[2];

	uint64_t xorshift128plus() {
		uint64_t x = s[0];
		uint64_t const y = s[1];
		s[0] = y;
		x ^= x << 23; // a
		s[1] = x ^ y ^ (x >> 17) ^ (y >> 26); // b, c
		return s[1] + y;
	}

	struct generator {
		using result_type = uint64_t;
		static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }
		static constexpr result_type min() { return std::numeric_limits<result_type>::min(); }
		result_type operator()() { return var.xorshift128plus(); }
		RandomizerXorshiftPlus& var;
	} gen{ *this };


	uint64_t m_rand;
	int counter;
	std::uniform_int_distribution<uint64_t> randomizer;
	std::random_device rd;
};