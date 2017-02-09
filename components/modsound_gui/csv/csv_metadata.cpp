#include "csv.h"
#include "csv_metadata.h"
#include "csv_load_entry.h"

#include "../sys/AppInfo.h"
#include "../common/io.h"
#include "../common/fs.h"

#include <windows.h>

CSV_Metadata_Globals* g_metadata = NULL;

//
// Enums
//
static const char* csv_enum_widgets[] =
{
	"text",
	"combo",
	"knob",
	"number",
	"check",
	"checklist",
	NULL
};

static const char* csv_enum_type[] =
{
	"bool",
	"int",
	"float",
	"string",
	"enum",
	"flags",
	NULL
};

const char *csv_enum_bus[4] =
{
	"world",
	"game",
	"voice",
	NULL
};

const char *csv_enum_looping[3] =
{
	"nonlooping",
	"looping",
	NULL
};

const char *csv_enum_priority[5] =
{
	"none",
	"oldest", 
	"reject",
	"priority",
	NULL
};

const char *csv_enum_move_type[9] =
{
	"none",
	"left_player",
	"center_player",
	"right_player",
	"random_player",
	"left_shot",
	"center_shot",
	"right_shot",
	NULL
};

const char *csv_enum_spatialized[4] =
{
	"2d",
	"3d",
	"2.5d",
	NULL
};

const char *csv_enum_storage_type[5] =
{
	"unknown",
	"loaded",
	"streamed",
	"primed",
	NULL
};

const char *csv_enum_yes_no[3] =
{
	"yes",
	"no",
	NULL
};

const char *csv_enum_yes_no_both[4] =
{
	"yes",
	"no",
	"both",
	NULL
};

const char* csv_enum_pc_format[2] =
{
	"wma",
	NULL
};


//
// Metadata entries
//
static csv_entry_t csv_entries_metadata[] =
{
	{ "table", offsetof(csv_metadata_s, table), true, CSV_STRING_CONST, 0.0, 32.0, 0, NULL },
	{ "column", offsetof(csv_metadata_s, column), true, CSV_STRING_CONST, 0.0, 32.0, 0, NULL },
	{ "title", offsetof(csv_metadata_s, title), true, CSV_STRING_CONST, 0.0, 64.0, 0, NULL },
	{ "tip", offsetof(csv_metadata_s, tip), false, CSV_STRING_CONST, 0.0, 256.0, 0, NULL },
	{ "wiget", offsetof(csv_metadata_s, widget), true, CSV_ENUM, 0.0, 0.0, 0, csv_enum_widgets },
	{ "type", offsetof(csv_metadata_s, type), true, CSV_ENUM, 0.0, 0.0, 0, csv_enum_type },
	{ "min", offsetof(csv_metadata_s, _min), false, CSV_FLOAT, 0.0, 0.0, 0, NULL },
	{ "max", offsetof(csv_metadata_s, _max), false, CSV_FLOAT, 0.0, 0.0, 0, NULL },
	{ "default", offsetof(csv_metadata_s, default_str), false, CSV_STRING_CONST, 0.0, 0.0, 0, NULL },
	{ "enumTable", offsetof(csv_metadata_s, enumTable), false, CSV_STRING_CONST, 0.0, 0.0, 0, NULL },
	{ "enumColumn", offsetof(csv_metadata_s, enumColumn), false, CSV_STRING_CONST, 0.0, 0.0, 0, NULL },
	{ "unit", offsetof(csv_metadata_s, unit), false, CSV_STRING_CONST, 0.0, 0.0, 0, NULL },
	{ "displayUnit", offsetof(csv_metadata_s, displayUnit), false, CSV_STRING_CONST, 0.0, 0.0, 0, NULL },
};

//
//
//
enum CSV_ENUM_TABLE_TYPE
{
	CSV_ENUM_TABLE_FILE_CSV,
	CSV_ENUM_TABLE_FILE_TXT,
	CSV_ENUM_TABLE_INTERNAL,
};

struct csv_enum_table_metadata_s
{
	const char* name;
	CSV_ENUM_TABLE_TYPE type;
	void* data;
};

//
// This must line up with CSV_ENUM_TABLE_TYPE
//
static csv_enum_table_metadata_s csv_metadata_enum_table_info_map[] =
{
	{ "bus",			CSV_ENUM_TABLE_INTERNAL, csv_enum_bus },
	{ "context",		CSV_ENUM_TABLE_FILE_CSV, "globals\\context.csv" },
	{ "curve",			CSV_ENUM_TABLE_FILE_CSV, "globals\\curves.csv" },
	{ "group",			CSV_ENUM_TABLE_FILE_CSV, "globals\\group.csv" },
	{ "limittype",		CSV_ENUM_TABLE_INTERNAL, csv_enum_priority },
	{ "loadspec",		CSV_ENUM_TABLE_FILE_TXT, "globals\\loadspec.txt" },
	{ "movetype",		CSV_ENUM_TABLE_INTERNAL, csv_enum_move_type },
	{ "pan",			CSV_ENUM_TABLE_FILE_CSV, "globals\\pan.csv" },
	{ "randomizeType",	CSV_ENUM_TABLE_INTERNAL, csv_enum_move_type },
	{ "snapshot",		CSV_ENUM_TABLE_FILE_CSV, "globals\\snapshot.csv" },
	{ "snapshotgroup",	CSV_ENUM_TABLE_FILE_CSV, "globals\\snapshot_groups.csv" },
	{ "spatialized",	CSV_ENUM_TABLE_INTERNAL, csv_enum_spatialized },
	{ "storage",		CSV_ENUM_TABLE_INTERNAL, csv_enum_storage_type },
	{ "template",		CSV_ENUM_TABLE_FILE_CSV, "template.csv" },
};

csv_enum_table_metadata_s* CSV_Metadata_Resolve_EnumTableMapping(const char* key)
{
	for (int i = 0; i < ARRAYSIZE(csv_metadata_enum_table_info_map); i++)
	{
		if (strcmp(key, csv_metadata_enum_table_info_map[i].name) == 0)
			return &csv_metadata_enum_table_info_map[i];
	}

	return 0;
}

csv_metadata_s::csv_metadata_s(void)
{
	memset(this, 0, sizeof(csv_metadata_s));
}

csv_metadata_s::~csv_metadata_s(void)
{
	if (table) free((void*)table);
	if (column) free((void*)column);
	if (title) free((void*)title);
	if (tip) free((void*)tip);

	if (default_str) free((void*)default_str);

	if (enumTable) free((void*)enumTable);
	if (enumColumn) free((void*)enumColumn);
	
	if (unit) free((void*)unit);
	if (displayUnit) free((void*)displayUnit);
}

CSV_Metadata_Globals::CSV_Metadata_Globals(void)
{
}

CSV_Metadata_Globals::~CSV_Metadata_Globals(void)
{
}

csv_metadata_s* CSV_Metadata_Globals::ResolveMetadataForField(const char* field_name)
{
	for (auto& metadata : this->metadata)
	{
		if (strcmp(field_name, metadata.column) == 0)
			return &metadata;
	}

	return 0;
}

int CSV_Metadata_Init()
{
	_ASSERT(g_metadata == NULL);

	Con_Printf("Init CSV system...\n");

	char path[MAX_PATH];
	sprintf_s(path, "%s\\raw\\soundaliases\\globals\\metadata.csv", AppInfo_AppDir());
	CSVStaticTable table(path, CSV_ST_PRUNE_EMPTY);

	//
	// Strip excess info as it may or may not be supported by the entry_set
	//
	for (int r = 0; r < table.RowCount();)
	{
		if (strcmp("alias", table.CellValue(r, 0)) != 0)
		{
			table.DeleteRow(r);
			continue;
		}

		r++;
	}
	
	g_metadata = new CSV_Metadata_Globals;

	g_metadata->metadata.clear();
	g_metadata->metadata.resize(table.RowCount()); // reserve enough spaces

	int count = 0;
	CSV_LoadEntry_StaticTable(&table, csv_entries_metadata, ARRAYSIZE(csv_entries_metadata), (BYTE*)&g_metadata->metadata[0], sizeof(csv_metadata_s), &count);

	// Note that resize does not change the capacity of the std::vector (ie the extra metadata remains allocated)
	g_metadata->metadata.resize(count);

	Con_Printf_v("Init default values...\n");
	for (auto& metadata : g_metadata->metadata)
	{
		const char* str = metadata.default_str;

		// Skip entries without default values (handled later)
		if (str == NULL)
			continue;

		switch (metadata.type)
		{
		case CSV_FIELD_TYPE::CSV_FIELD_BOOL:
			// Todo: Real handling for booleans
			metadata._default.boolean_value = (*str != '\0');
			break;
		case CSV_FIELD_TYPE::CSV_FIELD_INT:
			metadata._default.integer_value = atoi(str);
			break;
		case CSV_FIELD_TYPE::CSV_FIELD_FLOAT:
			metadata._default.float_value = (float)atof(str);
			break;
		case CSV_FIELD_TYPE::CSV_FIELD_STRING:
			// default_str is used
			break;
		case CSV_FIELD_TYPE::CSV_FIELD_ENUM:
			// todo... resolve the values
			metadata._default.enum_index = -1;
			break;
		case CSV_FIELD_TYPE::CSV_FIELD_FLAGS:
		default:
			Con_Error("Error: Unhandled field type (%d)\n", metadata.type);
			return 1;
		}
	}

	Con_Printf("Init enum tables...\n");

	std::vector<CSVStaticTable> enums;
	enums.resize(ARRAYSIZE(csv_metadata_enum_table_info_map)); // Allocate space for all possibly supported enum tables

	for (unsigned int i = 0; i < g_metadata->metadata.size(); i++)
	{
		if (g_metadata->metadata[i].type == CSV_FIELD_TYPE::CSV_FIELD_ENUM)
		{
			csv_enum_table_metadata_s* enum_table = CSV_Metadata_Resolve_EnumTableMapping(g_metadata->metadata[i].enumTable);
			if (!enum_table)
			{
				Con_Error("Unable to resolve enum info for enum table '%s'\n", g_metadata->metadata[i].enumTable);
				return 2;
			}

			//
			// Skip any enums tables that have already been initialized
			//
			if (g_metadata->enums[enum_table->name].Enums() != NULL)
				continue;

			int index = enum_table - csv_metadata_enum_table_info_map;
			int loadbits = CSV_ST_PRUNE_EMPTY;

			switch (enum_table->type)
			{
			case CSV_ENUM_TABLE_INTERNAL:
				//
				// Automatically load from the internal array
				//
				g_metadata->enums[enum_table->name].LoadFromInternalArray((const char**)enum_table->data);
				break;
			case CSV_ENUM_TABLE_FILE_TXT:
				loadbits |= CSV_ST_HEADERLESS_SINGLEFIELD;
			case CSV_ENUM_TABLE_FILE_CSV:
			{
				int index = enum_table - csv_metadata_enum_table_info_map;

				//
				// Cache the enum CSV if it hasnt already been loaded
				//
				if (enums[index].isEmpty())
				{
					sprintf_s(path, "%s\\raw\\soundaliases\\%s", AppInfo_AppDir(), (const char*)enum_table->data);
					enums[index].ReadFile(path, loadbits);
				}

				g_metadata->enums[enum_table->name].LoadFromTableColumn(&enums[index], g_metadata->metadata[i].enumColumn);
				break;
			}
			default:
				Con_Error("Error: Unhandled enum table type (error %d)\n", enum_table->type);
				return 3;
			}
		}
	}

	//
	// Initialize the hardcoded enums
	//

	return 0;
}

void CSV_Metadata_Free(void)
{
	_ASSERT(g_metadata);
	delete g_metadata;
}
