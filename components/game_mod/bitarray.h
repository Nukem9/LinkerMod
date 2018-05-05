#pragma once

template<size_t BIT_COUNT>
class bitarray
{
public:
	enum
	{
		BITS_PER_WORD	= sizeof(unsigned int) * 8,
		WORD_COUNT		= (BIT_COUNT + BITS_PER_WORD - 1) / BITS_PER_WORD,
	};

	// N bits -> array of size int(N/32) rounded up
	unsigned int array[WORD_COUNT];

	// /qcommon/bitarray.h:108
	bool testBit(unsigned int pos)
	{
		ASSERT(pos < BIT_COUNT);

		return (array[pos / BITS_PER_WORD] & (0x80000000 >> (pos & 0x1F))) != 0;
	}

	// /qcommon/bitarray.h:114
	void setBit(unsigned int pos)
	{
		ASSERT(pos < BIT_COUNT);

		array[pos / BITS_PER_WORD] |= (0x80000000 >> (pos & 0x1F));
	}
};