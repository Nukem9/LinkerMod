#include "csv.h"
#include "csv_alias.h"
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
	{ "mature",			CSV_ENUM_TABLE_INTERNAL, csv_enum_yes_no_both },
	{ "pc_format",		CSV_ENUM_TABLE_INTERNAL, csv_enum_pc_format },
};

csv_enum_table_metadata_s* CSV_Metadata_Resolve_EnumTableMapping(const char* key)
{
	if (!key)
		return 0;

	for (int i = 0; i < ARRAYSIZE(csv_metadata_enum_table_info_map); i++)
	{
		if (strcmp(key, csv_metadata_enum_table_info_map[i].name) == 0)
			return &csv_metadata_enum_table_info_map[i];
	}

	return 0;
}

csv_metadata_s::csv_metadata_s(void) : dont_free_strings(false)
{
	memset(this, 0, sizeof(csv_metadata_s));
}

// Move ctor - prevent freeing the strings from the object we took the pointers from
csv_metadata_s::csv_metadata_s(csv_metadata_s&& arg)
{
	memcpy(this, &arg, sizeof(csv_metadata_s));
	arg.dont_free_strings = true;
}

csv_metadata_s::~csv_metadata_s(void)
{
	delete[] enum_string;

	if (dont_free_strings)
		return;

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

void csv_metadata_s::MarkStringsAsStatic(void)
{
	this->dont_free_strings = true;
}
void csv_metadata_s::GenerateEnumString(void)
{
	_ASSERT(this->_enum);
	_ASSERT(this->enum_string == NULL);

	int buf_size = 1;
	for (int i = 0; _enum->Enums()[i]; i++)
	{
		buf_size += strlen(_enum->Enums()[i]) + 1;
	}

	this->enum_string = new char[buf_size];
	char* p = (char*)this->enum_string;
	for (const char** c =_enum->Enums(); *c; c++)
	{
		int len = sprintf_s(p, buf_size - (p - this->enum_string), "%s", *c);
		p += len + 1;
	}
	*p = '\0';
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

//
// Generate a metadate entry from its matching csv_entry_t (which is found via the field arg)
// returns 0 if successful
//
int CSV_MetaData_GenerateForAliasEntry(csv_metadata_s* meta, const char* field, const char* title, const char* tip, const char* default_str, const char* unit = "", const char* displayUnit="")
{
	meta->MarkStringsAsStatic();

	csv_entry_t* entry = CSV_ResolveAliasEntryFromFieldName(field);
	if (!entry)
	{
		Con_Error("ERROR: Unable to resolve entry for field '%s'\n", field);
		return 1;
	}

	meta->entry = entry;

	meta->table = "alias";
	meta->column = field;
	meta->title = title;
	meta->tip = tip;
	meta->default_str = default_str;
	meta->unit = unit;
	meta->displayUnit = displayUnit;

	switch (entry->type)
	{
	case CSV_DBSPL:
	case CSV_CENTS:
	case CSV_NORM_BYTE:
		meta->widget = CSV_WIDGET_KNOB;
		meta->type = CSV_FIELD_TYPE::CSV_FIELD_FLOAT;
		break;
	case CSV_FLAG:
		meta->widget = CSV_WIDGET_CHECKBOX;
		meta->type = CSV_FIELD_TYPE::CSV_FIELD_BOOL;
		break;
	case CSV_ENUM_BYTE:
		meta->widget = CSV_WIDGET_COMBO;
		meta->type = CSV_FIELD_TYPE::CSV_FIELD_ENUM;
		break;
	default:
		Con_Error("ERROR: Unable to resolve widget for field '%s' (err %d)\n", entry->type);
		return 2;
	}

	return 0;
}

//
// Add any builtin entries to g_metadata->metadata
//
int CSV_Metadata_Init_AddBuiltins(int* builtin_count)
{
	int err = 0;

	csv_metadata_s team_vol_mod;
	err |= CSV_MetaData_GenerateForAliasEntry(&team_vol_mod, "team_vol_mod", "Team Vol Mod", "", "100");
	
	csv_metadata_s team_pitch_mod;
	err |= CSV_MetaData_GenerateForAliasEntry(&team_pitch_mod, "team_pitch_mod", "Team Pitch Mod", "", "100");

	csv_metadata_s occlusion_wet_dry;
	err |= CSV_MetaData_GenerateForAliasEntry(&occlusion_wet_dry, "occlusion_wet_dry", "Occlusion Wet Dry", "", "100");

	csv_metadata_s real_delay;
	err |= CSV_MetaData_GenerateForAliasEntry(&real_delay, "real_delay", "Real Delay", "", "100");

	csv_metadata_s mature;
	err |= CSV_MetaData_GenerateForAliasEntry(&mature, "mature", "Mature", "", "");
	mature.enumTable = "mature";
	mature.enumColumn = "name";

	csv_metadata_s pc_format;
	err |= CSV_MetaData_GenerateForAliasEntry(&pc_format, "pc_format", "PC Format", "", "");
	pc_format.enumTable = "pc_format";
	pc_format.enumColumn = "name";

	if (err)
		return err;

	g_metadata->metadata.resize(6);
	g_metadata->metadata[0] = std::move(team_vol_mod);
	g_metadata->metadata[1] = std::move(team_pitch_mod);
	g_metadata->metadata[2] = std::move(occlusion_wet_dry);
	g_metadata->metadata[3] = std::move(real_delay);
	g_metadata->metadata[4] = std::move(mature);
	g_metadata->metadata[5] = std::move(pc_format);

	*builtin_count = g_metadata->metadata.size();
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

	int builtin_count = 0;
	if (int err = CSV_Metadata_Init_AddBuiltins(&builtin_count))
		return err;

	g_metadata->metadata.resize(table.RowCount() + builtin_count); // reserve enough spaces

	int count = 0;
	CSV_LoadEntry_StaticTable(&table, csv_entries_metadata, ARRAYSIZE(csv_entries_metadata), (BYTE*)&g_metadata->metadata[builtin_count], sizeof(csv_metadata_s), &count);

	//
	// Note that resize (to a smaller size) does not change the capacity of the std::vector (ie the extra metadata remains allocated)
	// If it did - it would invalidate the pointers
	//
	_ASSERT((unsigned int)(count + builtin_count) <= g_metadata->metadata.size());
	void* metadata = &g_metadata->metadata[0];
	g_metadata->metadata.resize(count + builtin_count);

	//
	// Ensure that the data didn't end up moving (if it did the pointers were invalidated)
	//
	_ASSERT(metadata == &g_metadata->metadata[0]);

	Con_Printf_v("Init default values...\n");
	for (auto& metadata : g_metadata->metadata)
	{
		metadata.entry = CSV_ResolveAliasEntryFromFieldName(metadata.column);
		if (!metadata.entry)
		{
			Con_Error("ERROR: Unable to resolve metadata entry for field '%s'\n", metadata.column);
			return 4;
		}

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

	//
	// Initialize the enum tables
	//
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
				Con_Error("Unable to resolve enum info for enum table '%s' (see '%'s)\n", g_metadata->metadata[i].enumTable, g_metadata->metadata[i].column);
				return 2;
			}

			//
			// Skip any enums tables that have already been initialized
			//
			if (g_metadata->enums[enum_table->name].Enums() != NULL)
			{
				g_metadata->metadata[i]._enum = &g_metadata->enums[enum_table->name];
				continue;
			}

			int index = enum_table - csv_metadata_enum_table_info_map;
			int loadbits = CSV_ST_PRUNE_EMPTY | CSV_ST_PRUNE_COMMENTS;

			switch (enum_table->type)
			{
			case CSV_ENUM_TABLE_INTERNAL:
				//
				// Automatically load from the internal array
				//
				g_metadata->enums[enum_table->name].LoadFromInternalArray((const char**)enum_table->data);
				g_metadata->metadata[i]._enum = &g_metadata->enums[enum_table->name];
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
				g_metadata->metadata[i]._enum = &g_metadata->enums[enum_table->name];
				break;
			}
			default:
				Con_Error("Error: Unhandled enum table type (error %d)\n", enum_table->type);
				return 3;
			}
		}
	}

	return 0;
}

void CSV_Metadata_Free(void)
{
	_ASSERT(g_metadata);
	delete g_metadata;
}
