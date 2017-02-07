#pragma once
#include "csv.h"
#include <wtypes.h>

enum csv_entry_type
{
	CSV_STRING,
	CSV_STRING_CONST,
	CSV_STRING_BUFFERED,
	CSV_FLOAT,
	CSV_INT,
	CSV_ENUM,
	CSV_FLAG,
	CSV_DBSPL,
	CSV_CENTS,
	CSV_HASH,
	CSV_ENUM_BITS,
	CSV_FLAG_BITS,
	CSV_BYTE,
	CSV_ENUM_BYTE,
	CSV_SHORT,
	CSV_USHORT,
	CSV_NORM_BYTE,

	CSV_IGNORE,
};

struct csv_entry_t
{
	const char *name;
	unsigned int offset;
	bool required;
	csv_entry_type type;
	float minimum;
	float maximum;
	unsigned int length;
	const char **enums;
};

int CSV_LoadEntry_StaticTable(const CSVStaticTable* table, csv_entry_t* entry_set, int entry_set_size, BYTE* dest, int dest_stride, int* count);
