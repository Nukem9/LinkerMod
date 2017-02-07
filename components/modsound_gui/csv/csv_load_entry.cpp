#include "csv_load_entry.h"
#include "../common/io.h"

#include <vector>

int CSV_LoadEntry_GetEnumIndex(const char* key, const char** enums)
{
	_ASSERT(enums);

	int result = -1;
	for (int i = 0; enums[i]; i++)
	{
		if (strcmp(key, enums[i]) == 0)
			return i;
	}

	return result;
}

int CSV_LoadEntry(const char* value, csv_entry_t* entry, BYTE* dst)
{
	//union
	//{
	//	char* str;
	//	int integer;
	//	float flt;
	//} data;

	if (value == NULL || *value == '\0')
	{
		return entry->required ? 1 : 0;
	}

	int val;

	switch (entry->type)
	{
	case CSV_STRING:
		(*(std::string*)(dst + entry->offset)) = value;
		return 0;
	case CSV_STRING_CONST:
		(*(const char**)(dst + entry->offset)) = _strdup(value);
		return 0;
	case CSV_FLOAT:
		*(float*)(dst + entry->offset) = (float)atof(value);
		return 0;
	case CSV_INT:
		*(int*)(dst + entry->offset) = atoi(value);
		return 0;
	case CSV_ENUM:
		val = CSV_LoadEntry_GetEnumIndex(value, entry->enums);
		if (val == -1)
		{
			return -1;
		}

		*(int*)(dst + entry->offset) = val;
		return 0;
	case CSV_FLAG:
	case CSV_DBSPL:
	case CSV_CENTS:
	case CSV_HASH:
	case CSV_ENUM_BITS:
	case CSV_FLAG_BITS:
	case CSV_BYTE:
	case CSV_ENUM_BYTE:
	case CSV_SHORT:
	case CSV_USHORT:
	case CSV_NORM_BYTE:

	case CSV_IGNORE:
		return 0;
	default:
		return -1;
	}

	return 0;
}

csv_entry_t* CSV_LoadEntry_ResolveEntry(const char* key, csv_entry_t* entry_set, int entry_set_size)
{
	for (int i = 0; i < entry_set_size; i++)
	{
		if (strcmp(key, entry_set[i].name) == 0)
			return &entry_set[i];
	}

	return NULL;
}

#include "csv_metadata.h"

int CSV_LoadEntry_StaticTable(const CSVStaticTable* table, csv_entry_t* entry_set, int entry_set_size, BYTE* dest, int dest_stride, int* count)
{
	_ASSERT(table);
	_ASSERT(entry_set);
	_ASSERT(dest);
	_ASSERT(count);

	*count = 0;

	std::vector<csv_entry_t*> field_map;
	for (int i = 0; i < table->FieldCount(); i++)
	{
		const char* field_name = table->FieldName(i);
		csv_entry_t* mapping = CSV_LoadEntry_ResolveEntry(field_name, entry_set, entry_set_size);
		if (!mapping)
		{
			Con_Warning("Unknown field %s \n", table->FieldName(i));
			continue;
		}
		field_map.push_back(mapping);
	}

	//
	// Todo: Verify that all required entries have been set up
	//
	for (int row_index = 0; row_index < table->RowCount(); row_index++)
	{
		//
		// Skip everything except the alias info right now
		//
		if (strcmp(table->CellValue(row_index, 0), "alias") != 0)
			break;

		for (int field_index = 0; field_index < table->FieldCount(); field_index++)
		{
			const char* key = table->CellValue(row_index, field_index);
			
			int err = CSV_LoadEntry(key, field_map[field_index], dest);
			if (err != 0)
			{
				Con_Error("Unable to load field '%s' for row %d (error %d)\n", table->FieldName(field_index), row_index, err);
				return err;
			}
		}

		(*count)++;
		dest += dest_stride;
	}

	return 0;
}
