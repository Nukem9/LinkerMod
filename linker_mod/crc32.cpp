#include "stdafx.h"

bool have_table = false;
uint32_t crc32_table[256];

void crc32_calctable()
{
	if (have_table)
		return;

	// Calculate CRC table
	for (int i = 0; i < ARRAYSIZE(crc32_table); i++)
	{
		uint32_t rem = i;// Remainder from polynomial division

		for (int j = 0; j < 8; j++)
		{
			if (rem & 1)
			{
				rem >>= 1;
				rem ^= 0xEDB88320;
			}
			else
			{
				rem >>= 1;
			}
		}

		crc32_table[i] = rem;
	}

	have_table = true;
}

uint32_t crc32_calculate(const char *buffer, size_t length, uint32_t crc)
{
	crc32_calctable();

	crc = ~crc;

	const char *p = nullptr;
	const char *q = buffer + length;

	for (p = buffer; p < q; p++)
	{
		unsigned __int8 octet = *p;// Cast to unsigned octet

		crc = (crc >> 8) ^ crc32_table[(crc & 0xff) ^ octet];
	}

	return ~crc;
}