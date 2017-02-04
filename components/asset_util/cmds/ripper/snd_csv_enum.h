#pragma once
#include <WTypes.h>
#include "foreign_ptr.h"

static const char* fields[74] =
{
	"name",
	"file",
	"template",
	"loadspec",
	"secondary",
	"group",
	"vol_min",
	"vol_max",
	"team_vol_mod",
	"dist_min",
	"dist_max",
	"dist_reverb_max",
	"volume_falloff_curve",
	"reverb_falloff_curve",
	"volume_min_falloff_curve",
	"reverb_min_falloff_curve",
	"limit_count",
	"limit_type",
	"entity_limit_count",
	"entity_limit_type",
	"pitch_min",
	"pitch_max",
	"team_pitch_mod",
	"min_priority",
	"max_priority",
	"min_priority_threshold",
	"max_priority_threshold",
	"spatialized",
	"type",
	"loop",
	"randomize_type",
	"probability",
	"start_delay",
	"reverb_send",
	"duck",
	"pan",
	"center_send",
	"envelop_min",
	"envelop_max",
	"envelop_percentage",
	"occlusion_level",
	"occlusion_wet_dry",
	"is_big",
	"distance_lpf",
	"move_type",
	"move_time",
	"real_delay",
	"subtitle",
	"mature",
	"doppler",
	"futz",
	"context_type",
	"context_value",
	"compression",
	"timescale",
	"music",
	"fade_in",
	"fade_out",
	"pc_format",
	"pause",
	"stop_on_death",
	"bus",
	"snapshot",
	"voice_limit",
	"file_xenon",
	"file_size_xenon",
	"file_ps3",
	"file_size_ps3",
	"file_pc",
	"file_size_pc",
	"file_wii",
	"file_size_wii",
	"source_csv",
	"language",
};

static const char *enum_bus[3] = { "world", "game", "voice" };
static const char *enum_type[5] = { "unknown", "loaded", "streamed", "primed", NULL };
static const char *enum_priority[5] = { "none", "oldest", "reject", "priority", NULL };

static const char *enum_move_type[9] =
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

static const char *enum_randomize_type[4] = { "volume", "pitch", "variant", NULL };
static const char *enum_spatialized[4] = { "2d", "3d", "2.5d", NULL };
static const char *enum_looping[3] = { "nonlooping", "looping", NULL };
static const char *enum_yes_no[3] = { "no", "yes", NULL };

struct snd_csv_enum_bits_entry_t
{
	int size;
	int shift;
	const char** enum_stringtable;
};

const char* SND_CSV_ResolveEnumBitsString(ForeignPointer<struct snd_alias_t>& alias, snd_csv_enum_bits_entry_t& enum_bits_entry);
unsigned int SND_CSV_ResolveEnumBitsValue(ForeignPointer<snd_alias_t>& alias, snd_csv_enum_bits_entry_t& enum_bits_entry);
