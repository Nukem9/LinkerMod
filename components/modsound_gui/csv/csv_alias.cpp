#include "csv_alias.h"

#define CSV_ENUM_MAPPING_STRUCT csv_snd_alias_t
#define CSV_ENUM_MAPPING(NAME, MEMBER, REQUIRED, TYPE, MIN, MAX, LENGTH, ENUM_IDS) \
	{ NAME, offsetof(CSV_ENUM_MAPPING_STRUCT, MEMBER), REQUIRED, TYPE, MIN, MAX, LENGTH, ENUM_IDS }



csv_entry_t csv_snd_alias_entries[] =
{
	CSV_ENUM_MAPPING("name", name, true, CSV_STRING_BUFFERED, 0.0f, 0.0f, CSV_SNDALIAS_NAME_LEN, NULL),
	CSV_ENUM_MAPPING("file", file, false, CSV_STRING_BUFFERED, 0.0f, 0.0f, CSV_SNDALIAS_FILE_LEN, NULL),
	CSV_ENUM_MAPPING("template", template_, false, CSV_STRING_BUFFERED, 0.0f, 0.0f, CSV_SNDALIAS_TEMPLATE_LEN, NULL),
	CSV_ENUM_MAPPING("loadspec", loadspec, false, CSV_STRING_BUFFERED, 0.0f, 0.0f, CSV_SNDALIAS_LOADSPEC_LEN, NULL),
	CSV_ENUM_MAPPING("secondary", secondary, false, CSV_STRING_BUFFERED, 0.0f, 0.0f, CSV_SNDALIAS_SECONDARY_LEN, NULL),
	CSV_ENUM_MAPPING("group", group, true, CSV_STRING_BUFFERED, 0.0f, 0.0f, CSV_SNDALIAS_GROUP_LEN, NULL),

	CSV_ENUM_MAPPING("vol_min", vol_min, false, CSV_DBSPL, 0.0f, 100.0f, 0, NULL),
	CSV_ENUM_MAPPING("vol_max", vol_max, false, CSV_DBSPL, 0.0f, 100.0f, 0, NULL),
	CSV_ENUM_MAPPING("team_vol_mod", team_vol_mod, false, CSV_DBSPL, 0.0f, 100.0f, 0, NULL),

	CSV_ENUM_MAPPING("dist_min", dist_min, false, CSV_USHORT, 0.0f, 65535.0f, 0, NULL),
	CSV_ENUM_MAPPING("dist_max", dist_max, false, CSV_USHORT, 0.0f, 65535.0f, 0, NULL),
	CSV_ENUM_MAPPING("dist_reverb_max", dist_reverb_max, false, CSV_USHORT, 0.0f, 65535.0f, 0, NULL),

	CSV_ENUM_MAPPING("volume_falloff_curve", volume_falloff_curve, false, CSV_ENUM_BYTE, 0.0f, 0.0f, 0, NULL), // CURVES
	CSV_ENUM_MAPPING("reverb_falloff_curve", reverb_falloff_curve, false, CSV_ENUM_BYTE, 0.0f, 0.0f, 0, NULL), // CURVES
	CSV_ENUM_MAPPING("volume_min_falloff_curve", volume_min_falloff_curve, false, CSV_ENUM_BYTE, 0.0f, 0.0f, 0, NULL), // CURVES
	CSV_ENUM_MAPPING("reverb_min_falloff_curve", reverb_min_falloff_curve, false, CSV_ENUM_BYTE, 0.0f, 0.0f, 0, NULL), // CURVES

	CSV_ENUM_MAPPING("limit_count", limit_count, false, CSV_BYTE, 0.0f, 128.0f, 0, NULL),
	CSV_ENUM_MAPPING("limit_type", limit_type, false, CSV_ENUM_BITS, 2.0f, 25.0f, 0, csv_enum_priority),

	CSV_ENUM_MAPPING("entity_limit_count", entity_limit_count, false, CSV_BYTE, 0.0f, 128.0f, 0, NULL),
	CSV_ENUM_MAPPING("entity_limit_type", entity_limit_type, false, CSV_ENUM_BITS, 2.0f, 27.0f, 0, csv_enum_priority),

	CSV_ENUM_MAPPING("pitch_min", pitch_min, false, CSV_CENTS, -2400.0f, 1200.0f, 0, NULL),
	CSV_ENUM_MAPPING("pitch_max", pitch_max, false, CSV_CENTS, -2400.0f, 1200.0f, 0, NULL),
	CSV_ENUM_MAPPING("team_pitch_mod", team_pitch_mod, false, CSV_CENTS, -2400.0f, 1200.0f, 0, NULL),

	CSV_ENUM_MAPPING("min_priority", min_priority, false, CSV_BYTE, 0.0f, 128.0f, 0, NULL),
	CSV_ENUM_MAPPING("max_priority", max_priority, false, CSV_BYTE, 0.0f, 128.0f, 0, NULL),
	CSV_ENUM_MAPPING("min_priority_threshold", min_priority_threshold, false, CSV_NORM_BYTE, 0.0f, 1.0f, 0, NULL),
	CSV_ENUM_MAPPING("max_priority_threshold", max_priority_threshold, false, CSV_NORM_BYTE, 0.0f, 1.0f, 0, NULL),

	CSV_ENUM_MAPPING("spatialized", spatialized, false, CSV_ENUM_BITS, 1.0f, 1.0f, 0, csv_enum_spatialized),
	CSV_ENUM_MAPPING("type", type, false, CSV_ENUM_BITS, 2.0f, 14.0f, 0, csv_enum_storage_type),
	CSV_ENUM_MAPPING("loop", loop, false, CSV_ENUM_BITS, 1.0f, 0.0f, 0, csv_enum_looping),
	CSV_ENUM_MAPPING("randomize_type", randomize_type, false, CSV_FLAG_BITS, 3.0f, 29.0f, 0, NULL), // RANDOMIZE_TYPE
	CSV_ENUM_MAPPING("probability", probability, false, CSV_NORM_BYTE, 0.0f, 1.0f, 0, NULL),
	CSV_ENUM_MAPPING("start_delay", start_delay, false, CSV_USHORT, 0.0f, 65535.0f, 0, NULL),
	CSV_ENUM_MAPPING("reverb_send", reverb_send, false, CSV_DBSPL, 0.0f, 100.0f, 0, NULL),

	CSV_ENUM_MAPPING("duck", duck, false, CSV_HASH, 0.0f, 0.0f, 0, NULL),
	CSV_ENUM_MAPPING("pan", pan, false, CSV_ENUM_BYTE, 0.0f, 0.0f, 0, NULL), // PANS

	CSV_ENUM_MAPPING("center_send", center_send, false, CSV_DBSPL, 0.0f, 100.0f, 0, NULL),

	CSV_ENUM_MAPPING("envelop_min", envelop_min, false, CSV_USHORT, 0.0f, 0.0f, 0, NULL),
	CSV_ENUM_MAPPING("envelop_max", envelop_max, false, CSV_USHORT, 0.0f, 0.0f, 0, NULL),
	CSV_ENUM_MAPPING("envelop_percentage", envelop_percentage, false, CSV_DBSPL, 0.0f, 100.0f, 0, NULL),

	CSV_ENUM_MAPPING("occlusion_level", occlusion_level, false, CSV_NORM_BYTE, 0.0f, 1.0f, 0, NULL),
	CSV_ENUM_MAPPING("occlusion_wet_dry", occlusion_wet_dry, false, CSV_NORM_BYTE, 0.0f, 1.0f, 0, NULL),

	CSV_ENUM_MAPPING("is_big", is_big, false, CSV_FLAG, 1.0f, 5.0f, 0, csv_enum_yes_no),
	CSV_ENUM_MAPPING("distance_lpf", distance_lpf, false, CSV_FLAG, 1.0f, 3.0f, 0, csv_enum_yes_no),
	CSV_ENUM_MAPPING("move_type", move_type, false, CSV_FLAG, 3.0f, 22.0f, 0, csv_enum_move_type),
	CSV_ENUM_MAPPING("move_time", move_time, false, CSV_USHORT, 0.0f, 65535.0f, 0, NULL),
	CSV_ENUM_MAPPING("real_delay", real_delay, false, CSV_FLAG, 1.0f, 2.0f, 0, csv_enum_yes_no),
	CSV_ENUM_MAPPING("subtitle", subtitle, false, CSV_STRING_BUFFERED, 0.0f, 0.0f, CSV_SNDALIAS_SUBTITLE_LEN, NULL),
	CSV_ENUM_MAPPING("mature", mature, false, CSV_ENUM_BYTE, 0.0f, 0.0f, 0, csv_enum_yes_no_both),

	CSV_ENUM_MAPPING("doppler", doppler, false, CSV_FLAG, 1.0f, 4.0f, 0, csv_enum_yes_no),
	CSV_ENUM_MAPPING("futz", futz, false, CSV_FLAG, 1.0f, 6.0f, 0, csv_enum_yes_no),

	CSV_ENUM_MAPPING("context_type", context_type, false, CSV_HASH, 0.0f, 0.0f, 0, NULL),
	CSV_ENUM_MAPPING("context_value", context_value, false, CSV_HASH, 0.0f, 0.0f, 0, NULL),

	CSV_ENUM_MAPPING("compression", compression, false, CSV_BYTE, 0.0f, 100.0f, 0, NULL),

	CSV_ENUM_MAPPING("timescale", timescale, false, CSV_FLAG, 1.0f, 10.0f, 0, csv_enum_yes_no),
	CSV_ENUM_MAPPING("music", music, false, CSV_FLAG, 1.0f, 8.0f, 0, csv_enum_yes_no),

	CSV_ENUM_MAPPING("fade_in", fade_in, false, CSV_BYTE, 0.0f, 0.0f, 0, NULL),
	CSV_ENUM_MAPPING("fade_out", fade_out, false, CSV_BYTE, 0.0f, 0.0f, 0, NULL),

	CSV_ENUM_MAPPING("pc_format", pc_format, false, CSV_ENUM_BYTE, 0.0f, 0.0f, 0, csv_enum_pc_format),

	CSV_ENUM_MAPPING("pause", pause, false, CSV_FLAG, 1.0f, 7.0f, 0, csv_enum_yes_no),
	CSV_ENUM_MAPPING("stop_on_death", stop_on_death, false, CSV_FLAG, 1.0f, 9.0f, 0, csv_enum_yes_no),
	CSV_ENUM_MAPPING("bus", bus, false, CSV_ENUM_BITS, 2.0f, 12.0f, 0, csv_enum_bus),
	CSV_ENUM_MAPPING("snapshot", snapshot, false, CSV_ENUM_BYTE, 0.0f, 0.0f, 0, NULL), // SNAPSHOTS
};

csv_entry_t* CSV_ResolveAliasEntryFromFieldName(const char* field_name)
{
	for (int i = 0; i < ARRAYSIZE(csv_snd_alias_entries); i++)
	{
		if (strcmp(field_name, csv_snd_alias_entries[i].name) == 0)
			return &csv_snd_alias_entries[i];
	}

	return NULL;
}

int CSV_LoadSoundAlias(const char* path, std::vector<csv_snd_alias_t>* aliases)
{
	const CSVStaticTable table(path, CSV_ST_PRUNE_EMPTY);
	return CSV_LoadSoundAlias(&table, aliases);
}

int CSV_LoadSoundAlias(const CSVStaticTable* table, std::vector<csv_snd_alias_t>* aliases)
{
	aliases->resize(table->RowCount());

	int count = 0;
	int r = CSV_LoadEntry_StaticTable(table, csv_snd_alias_entries, ARRAYSIZE(csv_snd_alias_entries), (BYTE*)&(*aliases)[0], sizeof(csv_snd_alias_t), &count);

	return r;
}
