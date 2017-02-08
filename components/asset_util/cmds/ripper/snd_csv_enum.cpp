#include "snd_csv_enum.h"
#include "snd_alias_db.h"
#include "foreign_ptr.h"

//
// Encode an enum's in a larger bitflags variable
// size is the number of bits to use for the input value
// shift is the number of bits to shift the masked input value
//
// For encoding enums from a snd_csv_entry_t
// 'size' is (int)entry->minimum
// 'shift' is (int)entry->maximum
//
void Snd_CSV_EncodeEnumBits(unsigned int* bits, unsigned int value, int size, int shift)
{
#if 0
	* bits = ((value & ((1 << size) - 1)) << shift) | ((*bits) & ~(((1 << size) - 1) << shift));
	return;
#endif

	unsigned int mask = (1 << size) - 1;
	unsigned int encoded_value = (value & mask) << shift;

	*bits = ((*bits) & ~(mask)) | encoded_value;
}

//
// Extract an encoded enum's value from a larger bitflags variable
// size is the number of bits to use for the input value
// shift is the number of bits to shift the masked input value
//
unsigned int Snd_CSV_DecodeEnumBits(unsigned int bits, int size, int shift)
{
	unsigned int mask = (1 << size) - 1;
	return (bits >> shift) & mask;
}

const char* SND_CSV_ResolveEnumBitsString(ForeignPointer<snd_alias_t>& alias, snd_csv_enum_bits_entry_t& enum_bits_entry)
{
	_ASSERT(enum_bits_entry.enum_stringtable);

	unsigned int bits = alias->flags;
	unsigned int enum_val = Snd_CSV_DecodeEnumBits(bits, enum_bits_entry.size, enum_bits_entry.shift);
	
	unsigned int enum_stringCount = 0;
	for (; enum_bits_entry.enum_stringtable[enum_stringCount]; enum_stringCount++);

	if (enum_val > enum_stringCount)
		return NULL;

	return enum_bits_entry.enum_stringtable[enum_val];
}

unsigned int SND_CSV_ResolveEnumBitsValue(ForeignPointer<snd_alias_t>& alias, snd_csv_enum_bits_entry_t& enum_bits_entry)
{
	unsigned int bits = alias->flags;
	return Snd_CSV_DecodeEnumBits(bits, enum_bits_entry.size, enum_bits_entry.shift);
}
