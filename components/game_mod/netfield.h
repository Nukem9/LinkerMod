#pragma once

struct NetField
{
	const char *name;
	int offset;
	int size;
	int bits;
	char changeHints;
#if 0
	const char *bitsStr;
	const char *changeHintsStr;
#endif
};
STATIC_ASSERT_SIZE(NetField, 0x14);

struct NetFieldList
{
	NetField *array;
	unsigned int count;
#if 0
	const char *fieldArrayName;
#endif
};
STATIC_ASSERT_SIZE(NetFieldList, 0x8);

void Cmd_DumpNetfield_f();