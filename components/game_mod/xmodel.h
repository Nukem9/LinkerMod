#pragma once

#define MAX_LODS 4

struct XModelHighMipBounds
{
};

struct XModel
{
	const char *name;
	char _pad1[0xE4];
	bool bad;
	char _pad2[0x13];
};
STATIC_ASSERT_OFFSET(XModel, name, 0x0);
STATIC_ASSERT_OFFSET(XModel, bad, 0xE8);
STATIC_ASSERT_SIZE(XModel, 0xFC);