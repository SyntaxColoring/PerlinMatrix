#include <cstdint>
#include "xxh32.hpp"

static std::uint32_t roll_left(std::uint32_t input, const unsigned int amount)
{
	std::uint32_t saved_bits = input >> (32-amount);

	input <<= amount;
	input |= saved_bits;

	return input;
}

std::uint32_t xxh_32(const std::uint32_t to_hash, const std::uint32_t seed)
{
	const std::uint32_t PRIME32_1 = 0x9E3779B1;
	const std::uint32_t PRIME32_2 = 0x85EBCA77;
	const std::uint32_t PRIME32_3 = 0xC2B2AE3D;
	const std::uint32_t PRIME32_4 = 0x27D4EB2F;
	const std::uint32_t PRIME32_5 = 0x165667B1;

	std::uint32_t acc = seed + PRIME32_5;

	// Add input length.
	acc += 4;

	// "Consume remaining input"
	// (which in our case is the entire input):
	acc += to_hash * PRIME32_3;
	acc = roll_left(acc, 17) * PRIME32_4;

	// Final mix (avalanche):
	acc ^= acc >> 15;
	acc *= PRIME32_2;
	acc ^= acc >> 13;
	acc *= PRIME32_3;
	acc ^= acc >> 16;

	return acc;
}
