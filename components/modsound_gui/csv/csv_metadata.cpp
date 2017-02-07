#include "csv.h"
#include "csv_metadata.h"
#include "csv_load_entry.h"

#include "../sys/AppInfo.h"
#include "../common/io.h"

#include <windows.h>

//
// Enums
//
const char* csv_enum_widgets[] =
{
	"text",
	"combo",
	"knob",
	"number",
	"check",
	"checklist",
	NULL
};

const char* csv_enum_type[] =
{
	"bool",
	"int",
	"float",
	"string",
	"enum",
	"flags",
	NULL
};

//
// Metadata entries
//
csv_entry_t csv_entries[] =
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

void CSV_Metadata_Init()
{
	Con_Printf("Init CSV system...\n");

	char path[MAX_PATH];
	sprintf_s(path, "%s\\raw\\soundaliases\\globals\\metadata.csv", AppInfo_AppDir());
	const CSVStaticTable table(path, true);

	std::vector<csv_metadata_s> data;
	data.resize(table.RowCount()); // reserve enough spaces

	int count = 0;
	CSV_LoadEntry_StaticTable(&table, csv_entries, ARRAYSIZE(csv_entries), (BYTE*)&data[0], sizeof(csv_metadata_s), &count);

	// Note that resize does not change the capacity of the std::vector (ie the extra metadata remains allocated)
	data.resize(count);

	/*
	enumTable

	bus						// static const char *enum_bus[3] = { "world", "game", "voice" };
	context					// globals/context.csv
	curve					// globals/curves.csv
	group					// globals/group.csv
	limittype				// static const char *enum_priority[5] = { "none", "oldest", "reject", "priority", NULL };
	loadspec				// globals/loadspec.txt
	movetype				// static (see snd_csv_enum.h) enum_move_type
	pan						// globals/pan.csv
	randomizeType			// static (see snd_csv_enum.h) enum_move_type
	snapshot				// globals/snapshot.csv
	snapshotgroup			// globals/snapshot_groups.csv
	spatialized				// static const char *enum_spatialized[4] = { "2d", "3d", "2.5d", NULL };
	storage					// static const char *enum_type[5] = { "unknown", "loaded", "streamed", "primed", NULL };
	template				// template.csv
	*/
	Con_Printf("Init enum tables...\n");

	//
	// Todo: Init enum tables
	//
}