#include "snd_ripper.h"
#include "../../common/fs.h"
#include "../../common/io.h"

#include "../../sys/AppInfo.h"
#include "../../cvar.h"

static snd_csv_enum_bits_entry_t ee_limit_type =			{ 2, 25, enum_priority };
static snd_csv_enum_bits_entry_t ee_entity_limit_type =		{ 2, 27, enum_priority };
static snd_csv_enum_bits_entry_t ee_randomize_type =		{ 3, 29, enum_randomize_type };
static snd_csv_enum_bits_entry_t ee_move_type =				{ 3, 22, enum_move_type };
static snd_csv_enum_bits_entry_t ee_type =					{ 2, 14, enum_type };
static snd_csv_enum_bits_entry_t ee_group =					{ 6, 16, NULL };
static snd_csv_enum_bits_entry_t ee_real_delay =			{ 1, 2, enum_yes_no };
static snd_csv_enum_bits_entry_t ee_distance_lpf =			{ 1, 3, enum_yes_no };
static snd_csv_enum_bits_entry_t ee_doppler =				{ 1, 4, enum_yes_no };
static snd_csv_enum_bits_entry_t ee_is_big =				{ 1, 5, enum_yes_no };
static snd_csv_enum_bits_entry_t ee_loop =					{ 1, 0, enum_looping };
static snd_csv_enum_bits_entry_t ee_spatialized =			{ 1, 1, enum_spatialized };
static snd_csv_enum_bits_entry_t ee_futz =					{ 1, 6, enum_yes_no };
static snd_csv_enum_bits_entry_t ee_music =					{ 1, 8, enum_yes_no };
static snd_csv_enum_bits_entry_t ee_timescale =				{ 1, 10, enum_yes_no };
static snd_csv_enum_bits_entry_t ee_pause =					{ 1, 7, enum_yes_no };
static snd_csv_enum_bits_entry_t ee_stop_on_death =			{ 1, 9, enum_yes_no };
static snd_csv_enum_bits_entry_t ee_bus =					{ 2, 12, enum_bus };
static snd_csv_enum_bits_entry_t ee_voice_limit =			{ 1, 11, enum_yes_no };

struct snd_csv_context_entry_t
{
	const char* type;
	const char* value[8];
};

static snd_csv_context_entry_t contexts[] =
{
	{"ringoff_plr",	{"indoor",		"outdoor",	NULL, NULL, NULL, NULL, NULL, NULL} },
	{"mature",		{"explicit",	"safe",		NULL, NULL, NULL, NULL, NULL, NULL} },
	{"test",		{"high",		"low",		NULL, NULL, NULL, NULL, NULL, NULL} },
	{"hazmat",		{"mask",		NULL,		NULL, NULL, NULL, NULL, NULL, NULL} },
};

//
// Read an external string with max length (len)
// if len == 0 - the string can be any length
//
std::string ReadString(const char* ptr, int len = 0)
{
	std::string out = "\0";
	SIZE_T numofbytesread;

	for (int i = 0; len == 0 || i < len; i++)
	{
		char c = 0;
		ReadProcessMemory(g_process.handle, (void*)ptr, (void*)&c, sizeof(c), &numofbytesread);

		if (c == '\0' || numofbytesread == 0)
			return out;

		out += c;
		ptr++;
	}

	return out;
}

double __cdecl SND_CSV_CENTS_Encode(double val)
{
	double v = val / 1200.0;
	return pow(2.0, v);
}

double SND_CSV_CENTS_Decode(double val)
{
	double v = log(val) / log(2);
	return v * 1200;
}

unsigned int __cdecl SND_HashName(const char *name)
{
	int hash = 0;

	if (name && *name)
	{
		unsigned int len = strlen(name);
		hash = 5381;
		for (unsigned i = 0; i < len; ++i)
			hash = (hash << 16) + (hash << 6) + tolower(name[i]) - hash;
		if (!hash)
			hash = 1;
	}

	return hash;
}

void Snd_CSV_PrintHeader_Soundalias(FILE* f)
{
	for (int i = 0; i < ARRAYSIZE(fields_soundalias); i++)
	{
		fprintf(f, "%s,", fields_soundalias[i]);
	}
	fprintf(f, "\n");
}

void Snd_CSV_PrintHeader_Snapshot(FILE* f, ForeignPointer<SndDriverGlobals>& globals)
{
	for (int i = 0; i < ARRAYSIZE(fields_snapshot); i++)
	{
		fprintf(f, "%s,", fields_snapshot[i]);
	}

	ForeignPointer<snd_snapshot_group> snapshotGroups(globals->snapshotGroups);
	for (unsigned int i = 0; i < globals->snapshotGroupCount; i++)
	{
		fprintf(f, "%s,", snapshotGroups[i].name);
	}

	fprintf(f, "\n");
}

void Snd_CSV_PrintHeader_Radverb(FILE* f)
{
	for (int i = 0; i < ARRAYSIZE(fields_radverb); i++)
	{
		fprintf(f, "%s,", fields_radverb[i]);
	}
	fprintf(f, "\n");
}

int Rip_Sound_Alias_Callback_f(ForeignPointer<snd_alias_t>& alias, snd_ripper_instance_info_t instance)
{
	std::string name = ReadString(alias->name, 256);
	std::string file = "";

	const char* _template = ""; //template
	const char* loadspec = ""; // unknown
	std::string secondaryname = ReadString(alias->secondaryname, 256); //secondary

	unsigned int group_index = SND_CSV_ResolveEnumBitsValue(alias, ee_group);
	_ASSERT(group_index < instance.globals->groupCount);

	ForeignPointer<snd_group> group((snd_group*)instance.globals->groups + group_index);

	unsigned short vol_min = alias->volMin;
	unsigned short vol_max = alias->volMax;
	unsigned short team_vol_mod = alias->teamVolMod;
	unsigned short dist_min = alias->distMin;
	unsigned short dist_max = alias->distMax;
	unsigned short dist_reverb_max = alias->distReverbMax;

	_ASSERT(alias->dryCurve < (int)instance.globals->curveCount);
	_ASSERT(alias->wetCurve < (int)instance.globals->curveCount);
	_ASSERT(alias->dryMinCurve < (int)instance.globals->curveCount);
	_ASSERT(alias->wetMinCurve < (int)instance.globals->curveCount);

	ForeignPointer<snd_curve> curves((snd_curve*)instance.globals->curves);

	const char* volume_falloff_curve = curves[alias->dryCurve].name;
	const char* reverb_falloff_curve = curves[alias->wetCurve].name;
	const char* volume_min_falloff_curve = curves[alias->dryMinCurve].name;
	const char* reverb_min_falloff_curve = curves[alias->wetMinCurve].name;

	char limit_count = alias->limitCount;
	const char* limit_type = SND_CSV_ResolveEnumBitsString(alias, ee_limit_type);
	char entity_limit_count = alias->entityLimitCount;
	const char* entity_limit_type = SND_CSV_ResolveEnumBitsString(alias, ee_entity_limit_type);
	unsigned short pitch_min = alias->pitchMin;
	unsigned short pitch_max = alias->pitchMax;
	unsigned short team_pitch_mod = alias->teamPitchMod;
	char min_priority = alias->minPriority;
	char max_priority = alias->maxPriority;
	unsigned char min_priority_threshold = alias->minPriorityThreshold;
	unsigned char max_priority_threshold = alias->maxPriorityThreshold;
	const char* spatialized = SND_CSV_ResolveEnumBitsString(alias, ee_spatialized);
	const char* type = SND_CSV_ResolveEnumBitsString(alias, ee_type);
	const char* loop = SND_CSV_ResolveEnumBitsString(alias, ee_loop);
	const char* randomize_type = SND_CSV_ResolveEnumBitsString(alias, ee_randomize_type);
	unsigned char probability = alias->probability;
	unsigned short start_delay = alias->startDelay;
	unsigned short reverb_send = alias->reverbSend;

	float fade_in = 0;
	float fade_out = 0;

	unsigned int duck_hash = alias->duck;
	std::string duck = (duck_hash) ? "<unknown>" : "";

	ForeignPointer<snd_snapshot> snapshots((snd_snapshot*)instance.bank->snapshots);
	if (duck_hash != 0)
	{
		for (auto& set : *instance.snapshots_map)
		{
			bool done = false;

			for (auto& snapshot : set.second)
			{
				if (duck_hash == SND_HashName(snapshot->name))
				{
					duck = snapshot->name;
					fade_in = snapshot->fadeIn;
					fade_out = snapshot->fadeOut;
					done = true;
					break;
				}
			}

			if (done)
				break;
		}
	}

	_ASSERT(duck != "<unknown>");

	unsigned char pan_index = alias->pan;
	_ASSERT(pan_index < instance.globals->panCount);

	ForeignPointer<snd_pan> pan = ((snd_pan*)instance.globals->pans + pan_index);

	unsigned short center_send = alias->centerSend;
	unsigned short envelop_min = alias->envelopMin;
	unsigned short envelop_max = alias->envelopMax;
	unsigned short envelop_percentage = alias->envelopPercentage;
	unsigned char occlusion_level = alias->occlusionLevel;
	unsigned char occlusion_wet_dry = alias->occlusionWetDry;
	const char* is_big = SND_CSV_ResolveEnumBitsString(alias, ee_is_big);
	const char* distance_lpf = SND_CSV_ResolveEnumBitsString(alias, ee_distance_lpf);
	const char* move_type = SND_CSV_ResolveEnumBitsString(alias, ee_move_type);
	unsigned short move_time = alias->fluxTime;
	const char* real_delay = SND_CSV_ResolveEnumBitsString(alias, ee_real_delay);
	std::string subtitle = ReadString(alias->subtitle, 1024);
	const char* mature = "both"; //mature
	const char* doppler = SND_CSV_ResolveEnumBitsString(alias, ee_doppler);
	const char* futz = SND_CSV_ResolveEnumBitsString(alias, ee_futz);

	unsigned int contextType_hash = alias->contextType; // context_type
	snd_csv_context_entry_t* context = NULL;

	for (int i = 0; i < ARRAYSIZE(contexts); i++)
	{
		if (contextType_hash == SND_HashName(contexts[i].type))
		{
			context = &contexts[i];
			break;
		}
	}

	_ASSERT(!contextType_hash || (contextType_hash && context));

	unsigned int contextValue_hash = alias->contextValue; //context_value
	const char* context_value = NULL;

	if (context != NULL)
	{
		for (int i = 0; i < 8; i++)
		{
			if (context->value[i] == NULL)
				break;

			if (contextValue_hash = SND_HashName(context->value[i]))
			{
				context_value = (*context).value[i];
				break;
			}
		}

		_ASSERT(context_value);
	}

	unsigned short compression = 0; //compression

	const char* timescale = SND_CSV_ResolveEnumBitsString(alias, ee_timescale);
	const char* music = SND_CSV_ResolveEnumBitsString(alias, ee_music);

	const char* pc_format = "xwma"; //pc_format - (This appears to be xwma in every existing alias - and theres no way to get it for non-loaded files)

	const char* pause = SND_CSV_ResolveEnumBitsString(alias, ee_pause);
	const char* stop_on_death = SND_CSV_ResolveEnumBitsString(alias, ee_stop_on_death);
	const char* bus = SND_CSV_ResolveEnumBitsString(alias, ee_bus);

	unsigned char snapshotGroup_index = alias->snapshotGroup;
	_ASSERT(snapshotGroup_index < instance.globals->snapshotGroupCount);

	ForeignPointer<snd_snapshot_group> snapshotgroup = ((snd_snapshot_group*)instance.globals->snapshotGroups + snapshotGroup_index);
	std::string snapshot = snapshotgroup->name;

	const char* voice_limit = SND_CSV_ResolveEnumBitsString(alias, ee_voice_limit);
	const char* file_xenon = "";
	int file_size_xenon = 0;
	const char* file_ps3 = "";
	int file_size_ps3 = 0;
	const char* file_pc = "";
	int file_size_pc = 0;
	const char* file_wii = "";
	int file_size_wii = 0;
	const char* source_csv = "";

	ForeignPointer<SoundFile> soundFile((SoundFile*)alias->soundFile);
	if (strcmp(type, "loaded") == 0)
	{
		ForeignPointer<LoadedSound> snd((LoadedSound*)soundFile->u.loadSnd);
		file = ReadString(snd->name);
	}
	else if (strcmp(type, "streamed") == 0)
	{
		ForeignPointer<StreamedSound> snd((StreamedSound*)soundFile->u.streamSnd);
		file = ReadString(snd->filename);
	}
	else if (strcmp(type, "primed") == 0)
	{
		ForeignPointer<StreamedSound> streamed_snd((StreamedSound*)soundFile->u.streamSnd);
		ForeignPointer<PrimedSound> primed_snd((PrimedSound*)streamed_snd->primeSnd);
		file = ReadString(primed_snd->name);
	}
	else // Try to use loaded since the name is always first
	{
		ForeignPointer<LoadedSound> snd((LoadedSound*)soundFile->u.loadSnd);
		file = ReadString(snd->name);
	}

	fprintf(instance.outputFile, "%s,", name.c_str());
	fprintf(instance.outputFile, "%s,", file.c_str());
	fprintf(instance.outputFile, "%s,", _template);
	fprintf(instance.outputFile, "%s,", loadspec);
	fprintf(instance.outputFile, "%s,", secondaryname.c_str());
	fprintf(instance.outputFile, "%s,", group->name);
	fprintf(instance.outputFile, "%.3g,", 100.0 * (double)vol_min / (double)USHRT_MAX);
	fprintf(instance.outputFile, "%.3g,", 100.0 * (double)vol_max / (double)USHRT_MAX);
	fprintf(instance.outputFile, "%.3g,", 100.0 * (double)team_vol_mod / (double)USHRT_MAX);
	fprintf(instance.outputFile, "%d,", dist_min);
	fprintf(instance.outputFile, "%d,", dist_max);
	fprintf(instance.outputFile, "%d,", dist_reverb_max);
	fprintf(instance.outputFile, "%s,", volume_falloff_curve);
	fprintf(instance.outputFile, "%s,", reverb_falloff_curve);
	fprintf(instance.outputFile, "%s,", volume_min_falloff_curve);
	fprintf(instance.outputFile, "%s,", reverb_min_falloff_curve);
	fprintf(instance.outputFile, "%d,", limit_count);
	fprintf(instance.outputFile, "%s,", limit_type);
	fprintf(instance.outputFile, "%d,", entity_limit_count);
	fprintf(instance.outputFile, "%s,", entity_limit_type);
	fprintf(instance.outputFile, "%.3g,", SND_CSV_CENTS_Decode((double)pitch_min / (double)SHRT_MAX));
	fprintf(instance.outputFile, "%.3g,", SND_CSV_CENTS_Decode((double)pitch_max / (double)SHRT_MAX));
	fprintf(instance.outputFile, "%.3g,", SND_CSV_CENTS_Decode((double)team_pitch_mod / (double)SHRT_MAX));
	fprintf(instance.outputFile, "%d,", min_priority);
	fprintf(instance.outputFile, "%d,", max_priority);
	fprintf(instance.outputFile, "%.3g,", (double)min_priority_threshold / (double)UCHAR_MAX);
	fprintf(instance.outputFile, "%.3g,", (double)max_priority_threshold / (double)UCHAR_MAX);
	fprintf(instance.outputFile, "%s,", spatialized);
	fprintf(instance.outputFile, "%s,", type);
	fprintf(instance.outputFile, "%s,", loop);
	fprintf(instance.outputFile, "%s,", randomize_type);
	fprintf(instance.outputFile, "%d,", (double)probability / (double)UCHAR_MAX);
	fprintf(instance.outputFile, "%d,", start_delay);
	fprintf(instance.outputFile, "%.3g,", 100.0 * (double)reverb_send / (double)USHRT_MAX);
	fprintf(instance.outputFile, "%s,", duck.c_str());
	fprintf(instance.outputFile, "%s,", pan->name);
	fprintf(instance.outputFile, "%.3g,", 100.0 * (double)center_send / (double)USHRT_MAX);
	fprintf(instance.outputFile, "%d,", envelop_min);
	fprintf(instance.outputFile, "%d,", envelop_max);
	fprintf(instance.outputFile, "%.3g,", 100 * (double)envelop_percentage / (double)USHRT_MAX);
	fprintf(instance.outputFile, "%.3g,", (double)occlusion_level / (double)UCHAR_MAX);
	fprintf(instance.outputFile, "%.3g,", (double)occlusion_wet_dry / (double)UCHAR_MAX);
	fprintf(instance.outputFile, "%s,", is_big);
	fprintf(instance.outputFile, "%s,", distance_lpf);
	fprintf(instance.outputFile, "%s,", move_type);
	fprintf(instance.outputFile, "%d,", move_time);
	fprintf(instance.outputFile, "%s,", real_delay);
	fprintf(instance.outputFile, "%s,", subtitle.c_str());
	fprintf(instance.outputFile, "%s,", mature);
	fprintf(instance.outputFile, "%s,", doppler);
	fprintf(instance.outputFile, "%s,", futz);
	fprintf(instance.outputFile, "%s,", context ? context->type : ""); // , context_type);
	fprintf(instance.outputFile, "%s,", context_value ? context_value : ""); // , context_value);
	fprintf(instance.outputFile, "%d,", compression);
	fprintf(instance.outputFile, "%s,", timescale);
	fprintf(instance.outputFile, "%s,", music);
	fprintf(instance.outputFile, "%f,", fade_in);
	fprintf(instance.outputFile, "%f,", fade_out);
	fprintf(instance.outputFile, "%s,", pc_format);
	fprintf(instance.outputFile, "%s,", pause);
	fprintf(instance.outputFile, "%s,", stop_on_death);
	fprintf(instance.outputFile, "%s,", bus);
	fprintf(instance.outputFile, "%s,", snapshot.c_str());
	fprintf(instance.outputFile, "%s,", voice_limit);
	fprintf(instance.outputFile, "%s,", file_xenon);
	fprintf(instance.outputFile, "%d,", file_size_xenon);
	fprintf(instance.outputFile, "%s,", file_ps3);
	fprintf(instance.outputFile, "%d,", file_size_ps3);
	fprintf(instance.outputFile, "%s,", file_pc);
	fprintf(instance.outputFile, "%d,", file_size_pc);
	fprintf(instance.outputFile, "%s,", file_wii);
	fprintf(instance.outputFile, "%d,", file_size_wii);
	fprintf(instance.outputFile, "%s,", source_csv);
	fprintf(instance.outputFile, "%s,\n", instance.language.c_str());

	return 0;
}

std::string Snd_ResolveBaseName(std::string& str)
{
	auto offset = str.find(".");

	if (offset != std::string::npos)
	{
		return std::string(str, 0, offset);
	}

	return str;
}

int Rip_SoundBank_GatherSnapshots_Callback_f(ForeignPointer<XAsset>& asset, ForeignPointer<XZoneName>& zoneName, void* data)
{
	_ASSERT(data);

	snapshots_map_t* snapshots_map = (snapshots_map_t*)data;

	ForeignPointer<SndBank> bank((SndBank*)asset->header.sound);
	auto& snapshots = (*snapshots_map)[Snd_ResolveBaseName(ReadString(bank->name))];

	if (bank->snapshots == nullptr || bank->snapshotCount == 0)
		return 0;

	for (unsigned int i = 0; i < bank->snapshotCount; i++)
	{
		snapshots.push_back(ForeignPointer<snd_snapshot>((snd_snapshot*)bank->snapshots + i));
	}

	return 0;
}

int Rip_SoundBank_GatherRadverbs_Callback_f(ForeignPointer<XAsset>& asset, ForeignPointer<XZoneName>& zoneName, void* data)
{
	_ASSERT(data);

	radverbs_map_t* radverbs_map = (radverbs_map_t*)data;

	ForeignPointer<SndBank> bank((SndBank*)asset->header.sound);
	auto& radverbs = (*radverbs_map)[Snd_ResolveBaseName(ReadString(bank->name))];

	if (bank->radverbs == nullptr || bank->radverbCount == 0)
		return 0;

	for (unsigned int i = 0; i < bank->radverbCount; i++)
	{
		radverbs.push_back(ForeignPointer<snd_radverb>(bank->radverbs + i));
	}

	return 0;
}

int Rip_SoundBank_Soundalias_Callback_f(ForeignPointer<SndBank>& bank, ForeignPointer<SndDriverGlobals>& globals, snapshots_map_t* snapshots_map)
{
	std::string name = ReadString(bank->name, 128);

	Con_Print("Exporting soundalias %s.csv...\n", name.c_str());

	int err = FS_CreatePath("soundaliases\\zones\\");
	if (err)
	{
		return Con_Error("Error: Unable to create output path (0x%X)\n", err);
	}

	char path[MAX_PATH];
	sprintf_s(path, "%s/soundaliases\\zones\\%s.csv", AppInfo_OutDir(), name.c_str());

	FS_SanitizePath(path);

	if (FS_FileExists(path) && !fs_overwrite.ValueBool())
	{
		Con_Print("  ...skipping (file already exists)\n");
		return 1;
	}

	FILE* h = fopen(path, "w");

	if (!h)
	{
		return Con_Error("Couldnt open '%s'\n", path);
	}

	Snd_CSV_PrintHeader_Soundalias(h);

	std::string language = "<error>";

	auto _offset = name.find_last_of(".");
	if (_offset != std::string::npos && _offset + 1 < name.size())
		language = std::string(name, _offset + 1);
	else
		Con_Warning("Unable to determine language for '%s'\n", name.c_str());

	snd_ripper_instance_info_t instance = { h, language, globals, bank, snapshots_map };

	ForeignPointer<snd_alias_list_t> alias((snd_alias_list_t*)bank->alias);
	for (unsigned int i = 0; i < bank->aliasCount; i++)
	{
		ForeignPointer<snd_alias_t> entry((snd_alias_t*)alias[i].head);
		Rip_Sound_Alias_Callback_f(entry, instance);
	}

	fclose(h);
	return 0;
}

int Rip_SoundBank_Callback_f(ForeignPointer<XAsset>& asset, ForeignPointer<XZoneName>& zoneName, void* data)
{
	ForeignPointer<SndDriverGlobals> globals((SndDriverGlobals*)DB_FindSingletonAssetForType(ASSET_TYPE_SNDDRIVER_GLOBALS));
	ForeignPointer<SndBank> bank((SndBank*)asset->header.sound);

	return Rip_SoundBank_Soundalias_Callback_f(bank, globals, (snapshots_map_t*)data);
}


int Rip_Snapshot_Callback_f(std::string name, std::vector<ForeignPointer<snd_snapshot>>& snapshots)
{
	Con_Print("Exporting snapshot %s.snapshot.csv...\n", name.c_str());

	int err = FS_CreatePath("soundaliases\\zones\\snapshots\\");
	if (err)
	{
		return Con_Error("Error: Unable to create output path (0x%X)\n", err);
	}

	char path[MAX_PATH];
	sprintf_s(path, "%s/soundaliases\\zones\\snapshots\\%s.csv", AppInfo_OutDir(), name.c_str());

	FS_SanitizePath(path);

	if (FS_FileExists(path) && !fs_overwrite.ValueBool())
	{
		Con_Print("  ...skipping (file already exists)\n");
		return 1;
	}

	FILE* h = fopen(path, "w");

	if (!h)
	{
		return Con_Error("Couldnt open '%s'\n", path);
	}

	ForeignPointer<SndDriverGlobals> globals((SndDriverGlobals*)DB_FindSingletonAssetForType(ASSET_TYPE_SNDDRIVER_GLOBALS));
	Snd_CSV_PrintHeader_Snapshot(h, globals);

	for (auto& snapshot : snapshots)
	{
		fprintf(h, "%s,", snapshot->name);
		fprintf(h, "%s,", snapshot->occlusionName);
		fprintf(h, ","); // loadspec
		fprintf(h, "%.3g,", snapshot->fadeIn);
		fprintf(h, ","); // fadeInCurve
		fprintf(h, "%.3g,", snapshot->fadeOut);
		fprintf(h, ","); // fadeOutCurve
		fprintf(h, "%d,", (int)snapshot->distance);

		for (unsigned int i = 0; i < globals->snapshotGroupCount; i++)
		{
			fprintf(h, "%.3g,", snapshot->attenuation[i]);
		}

		fprintf(h, "\n");
	}

	fclose(h);
	return 0;
}

int Rip_Radverb_Callback_f(std::string name, std::vector<ForeignPointer<snd_radverb>>& radverbs)
{
	Con_Print("Exporting snapshot %s.radverb.csv...\n", name.c_str());

	int err = FS_CreatePath("soundaliases\\zones\\radverb\\");
	if (err)
	{
		return Con_Error("Error: Unable to create output path (0x%X)\n", err);
	}

	char path[MAX_PATH];
	sprintf_s(path, "%s/soundaliases\\zones\\radverb\\%s.csv", AppInfo_OutDir(), name.c_str());

	FS_SanitizePath(path);

	if (FS_FileExists(path) && !fs_overwrite.ValueBool())
	{
		Con_Print("  ...skipping (file already exists)\n");
		return 1;
	}

	FILE* h = fopen(path, "w");

	if (!h)
	{
		return Con_Error("Couldnt open '%s'\n", path);
	}

	Snd_CSV_PrintHeader_Radverb(h);

	for (auto& radverb : radverbs)
	{
		fprintf(h, "%s,", radverb->name);
		fprintf(h, ","); //loadspec
		fprintf(h, "%.7g,", radverb->smoothing);
		fprintf(h, "%.7g,", radverb->earlyTime);
		fprintf(h, "%.7g,", radverb->lateTime);
		fprintf(h, "%.7g,", radverb->earlyGain);
		fprintf(h, "%.7g,", radverb->lateGain);
		fprintf(h, "%.7g,", radverb->returnGain); 
		fprintf(h, "%.7g,", radverb->earlyLpf);
		fprintf(h, "%.7g,", radverb->lateLpf);
		fprintf(h, "%.7g,", radverb->inputLpf);
		fprintf(h, "%.7g,", radverb->dampLpf);
		fprintf(h, "%.7g,", radverb->wallReflect);
		fprintf(h, "%.7g,", radverb->dryGain);
		fprintf(h, "%.7g,", radverb->earlySize);
		fprintf(h, "%.7g,", radverb->lateSize);
		fprintf(h, "%.7g,", radverb->diffusion);

		fprintf(h, "\n");
	}

	fclose(h);
	return 0;
}

