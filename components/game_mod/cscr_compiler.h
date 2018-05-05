#pragma once

#define SCR_FUNC_TABLE_SIZE 1024

enum
{
	SCR_DEV_NO = 0x0,
	SCR_DEV_YES = 0x1,
	SCR_DEV_IGNORE = 0x2,
	SCR_DEV_EVALUATE = 0x3,
};

struct scrCompileGlob_t
{
};

struct scrCompilePub_t
{
	int value_count;							// 0x30257B0
	int far_function_count;						// 0x30257B4
	unsigned int loadedscripts;					// 0x30257B8
	unsigned int scriptsPos;					// 0x30257BC
	unsigned int scriptsCount;					// 0x30257C0
	unsigned int builtinFunc;					// 0x30257C4
	unsigned int builtinMeth;					// 0x30257C8
	unsigned __int16 canonicalStrings[65536];	// 0x30257CC
	const char *in_ptr;							// ?
	const char *parseBuf;						// ?
	bool script_loading;						// 0x30457D4
	bool allowedBreakpoint;						// 0x30457D5
	int developer_statement;					// 0x30457D8
	char *opcodePos;							// 0x30457DC
	unsigned int programLen;					// 0x30457E0
	int func_table_size;						// 0x30457E4
	int func_table[SCR_FUNC_TABLE_SIZE];		// 0x30457E8
};
STATIC_ASSERT_SIZE(scrCompilePub_t, 0x21038);

static scrCompilePub_t *gScrCompilePub = (scrCompilePub_t *)0x030257B0;