#pragma once

template<size_t BIT_COUNT>
class bitarray
{
	static_assert(BIT_COUNT == 51, "Class doesn't support other bit sizes with current code");

public:
	enum
	{
		BITS_PER_WORD	= 32,
		WORD_COUNT		= (BIT_COUNT + BITS_PER_WORD - 1) / BITS_PER_WORD,
	};

	// N bits -> array of size int(N/32) rounded up
	int array[WORD_COUNT];

	// /qcommon/bitarray.h:108
	bool testBit(unsigned int pos)
	{
		ASSERT(pos < BIT_COUNT);

		return (this->array[pos >> 5] & (0x80000000 >> (pos & 0x1F))) != 0;
	}
};