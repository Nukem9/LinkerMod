#pragma once
#include "csv.h"

class CSVEnumInfo
{
private:
	bool is_static_array;
	const char** enums;

public:
	//
	// Todo: Add Specialized values for the context_type
	//
	CSVEnumInfo(void);
	~CSVEnumInfo(void);

	const char** Enums(void) const;

	int LoadFromTableColumn(CSVStaticTable* table, const char* column);
	int LoadFromInternalArray(const char** _enums);
};
