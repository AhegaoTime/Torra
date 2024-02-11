#include "uint32_uint8_converter.h"

uint32_t uint32_uint8_converter::uint8_to_uint32(const uint8_t* uint8) {
	uint32_t uint32 = 0;

	for (uint8_t counter = 0; counter < UINT8_PER_UINT32; counter++) {
		uint32 += uint8[counter]; //Sets new uint8 as the rightmost byte.
		if (counter < (UINT8_PER_UINT32 - 1)) { //We don't want to shift left in the last time.
			uint32 <<= BITS_PER_BYTE; //Move everything 1 byte left.
		}
	}

	return uint32;
}

void uint32_uint8_converter::uint32_to_uint8(uint32_t uint32, uint8_t* uint8) {
	for (uint8_t counter = UINT8_PER_UINT32; counter > 0; counter--) {
		uint8[counter - 1] = uint32 & 0xFF; //Gets rightmost byte.
		uint32 >>= BITS_PER_BYTE; //Move 1 byte to the right.
	}
}