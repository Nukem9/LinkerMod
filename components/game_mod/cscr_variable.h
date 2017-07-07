#pragma once

struct scrVarPub_t
{
	const char *fieldBuffer;					// 0x32C8680
	unsigned __int16 canonicalStrCount;			// 0x32C8684
	bool developer;								// 0x32C8686
	bool developer_script;						// 0x32C8687
	char _pad1[0x30];
	unsigned int checksum;						// 0x32C86B8
	char _pad2[0xC];
	const char *programBuffer;					// 0x32C86C8
	const char *endScriptBuffer;				// 0x32C86CC
	char _pad3[0x8];
};
STATIC_ASSERT_SIZE(scrVarPub_t, 0x58);

static scrVarPub_t *gScrVarPub = (scrVarPub_t *)0x032C8680;

void Scr_GetChecksumNoAsserts(int *checksum, scriptInstance_t inst);