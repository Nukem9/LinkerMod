#include "csv_enum.h"

CSVEnumInfo::CSVEnumInfo(void) : enums(NULL), is_static_array(false)
{
}

CSVEnumInfo::~CSVEnumInfo(void)
{
	if (!is_static_array && this->enums != NULL)
	{
		for (int i = 0; enums[i]; i++)
			free((void*)enums[i]);
		delete[] enums;
	}
}

const char** CSVEnumInfo::Enums(void) const
{
	return this->enums;
}

int CSVEnumInfo::LoadFromTableColumn(CSVStaticTable* table, const char* column)
{
	_ASSERT(enums == NULL);

	for (int c = 0; c < table->FieldCount(); c++)
	{
		if (strcmp(column, table->FieldName(c)) == 0)
		{
			this->enums = new const char*[table->RowCount() + 1];
			enums[table->RowCount()] = NULL;
			for (int r = 0; r < table->RowCount(); r++)
			{
				this->enums[r] = _strdup(table->CellValue(r, c));
			}
		}
	}

	return 0;
}


int CSVEnumInfo::LoadFromInternalArray(const char** _enums)
{
	_ASSERT(enums == NULL);
	this->is_static_array = true;
	this->enums = _enums;
	return 0;
}