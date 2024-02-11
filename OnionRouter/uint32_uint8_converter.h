#pragma once
#include <cstdint>

static constexpr const uint8_t UINT8_PER_UINT32 = 4;
static constexpr const uint8_t BITS_PER_BYTE = 8;

//Static class, contains functions the convert uint32_t to 4 uint8_t and backwards.
class uint32_uint8_converter {
public:
	static uint32_t uint8_to_uint32(const uint8_t* uint8); //Converts array of 4 uint8_t to 1 uint32_t.
	static void uint32_to_uint8(uint32_t uint32, uint8_t* uint8); //Convert 1 uint32_t to array of 4 uint8_t.
};