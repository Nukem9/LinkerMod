#pragma once
#include "csv.h"
#include "csv_metadata.h"
#include "csv_load_entry.h"

#include <string>

typedef BYTE enum_byte_t;
typedef BYTE enum_bits_t;
typedef float norm_byte_t;
typedef bool flag_t;

#define CSV_SNDALIAS_NAME_LEN 64
#define CSV_SNDALIAS_FILE_LEN MAX_PATH
#define CSV_SNDALIAS_TEMPLATE_LEN 32
#define CSV_SNDALIAS_LOADSPEC_LEN 32
#define CSV_SNDALIAS_SECONDARY_LEN CSV_SNDALIAS_NAME_LEN
#define CSV_SNDALIAS_GROUP_LEN 64

#define CSV_SNDALIAS_SUBTITLE_LEN 64

struct csv_snd_alias_t
{
	char name[CSV_SNDALIAS_NAME_LEN];
	char file[CSV_SNDALIAS_FILE_LEN];
	char template_[CSV_SNDALIAS_TEMPLATE_LEN];
	char loadspec[CSV_SNDALIAS_LOADSPEC_LEN];
	char secondary[CSV_SNDALIAS_SECONDARY_LEN];
	char group[CSV_SNDALIAS_GROUP_LEN];
	float vol_min;
	float vol_max;
	float team_vol_mod;
	unsigned short dist_min;
	unsigned short dist_max;
	unsigned short dist_reverb_max;
	enum_byte_t volume_falloff_curve;
	enum_byte_t reverb_falloff_curve;
	enum_byte_t volume_min_falloff_curve;
	enum_byte_t reverb_min_falloff_curve;
	BYTE limit_count;
	enum_byte_t limit_type;
	BYTE entity_limit_count;
	enum_bits_t entity_limit_type;
	float pitch_min;
	float pitch_max;
	float team_pitch_mod;
	BYTE min_priority;
	BYTE max_priority;
	norm_byte_t min_priority_threshold;
	norm_byte_t max_priority_threshold;
	enum_bits_t spatialized;	
	enum_bits_t type;			
	enum_bits_t loop;			
	char randomize_type; // flag bits
	norm_byte_t probability;
	unsigned short start_delay;
	float reverb_send;
	char duck; // hash
	enum_byte_t pan;
	float center_send;
	unsigned short envelop_min;
	unsigned short envelop_max;
	float envelop_percentage;
	norm_byte_t occlusion_level;
	norm_byte_t occlusion_wet_dry;
	bool is_big;
	bool distance_lpf;
	enum_bits_t move_type;
	unsigned short move_time;
	flag_t real_delay;
	char subtitle[CSV_SNDALIAS_SUBTITLE_LEN];
	enum_byte_t mature;
	flag_t doppler;
	flag_t futz;
	enum_bits_t context_type;
	enum_bits_t context_value; 
	BYTE compression;
	flag_t timescale;
	flag_t music;
	BYTE fade_in;	// always 0 - deffered to snapshot settings
	BYTE fade_out;	// always 0 - deffered to snapshot settings
	enum_byte_t pc_format;
	flag_t pause;
	bool stop_on_death;
	char bus;
	enum_byte_t snapshot;
};

csv_entry_t* CSV_ResolveAliasEntryFromFieldName(const char* field_name);

int CSV_LoadSoundAlias(const char* path, std::vector<csv_snd_alias_t>* aliases);
int CSV_LoadSoundAlias(const CSVStaticTable* table, std::vector<csv_snd_alias_t>* aliases);
