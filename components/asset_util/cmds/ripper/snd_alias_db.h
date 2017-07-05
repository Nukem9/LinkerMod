#pragma once

enum snd_asset_format
{
	SND_ASSET_FORMAT_PCMS16 = 0x0,
	SND_ASSET_FORMAT_PCMS24 = 0x1,
	SND_ASSET_FORMAT_PCMS32 = 0x2,
	SND_ASSET_FORMAT_IEEE = 0x3,
	SND_ASSET_FORMAT_XMA4 = 0x4,
	SND_ASSET_FORMAT_MP3 = 0x5,
	SND_ASSET_FORMAT_MSADPCM = 0x6,
	SND_ASSET_FORMAT_WMA = 0x7,
};

enum snd_asset_channel
{
	SND_ASSET_CHANNEL_L = 0x1,
	SND_ASSET_CHANNEL_R = 0x2,
	SND_ASSET_CHANNEL_C = 0x4,
	SND_ASSET_CHANNEL_LFE = 0x8,
	SND_ASSET_CHANNEL_LS = 0x10,
	SND_ASSET_CHANNEL_RS = 0x20,
	SND_ASSET_CHANNEL_LB = 0x40,
	SND_ASSET_CHANNEL_RB = 0x80,
};

enum snd_asset_flags
{
	SND_ASSET_FLAG_DEFAULT = 0x0,
	SND_ASSET_FLAG_LOOPING = 0x1,
	SND_ASSET_FLAG_PAD_LOOP_BUFFER = 0x2,
};

struct snd_radverb
{
	char name[32];
	unsigned int id;
	float smoothing;
	float earlyTime;
	float lateTime;
	float earlyGain;
	float lateGain;
	float returnGain;
	float earlyLpf;
	float lateLpf;
	float inputLpf;
	float dampLpf;
	float wallReflect;
	float dryGain;
	float earlySize;
	float lateSize;
	float diffusion;
};

struct snd_snapshot
{
	char name[32];
	unsigned int id;
	char occlusionName[32];
	unsigned int occlusionId;
	float fadeIn;
	float fadeOut;
	float distance;
	unsigned int fadeInCurve;
	unsigned int fadeOutCurve;
	float attenuation[64];
};

struct snd_asset
{
	unsigned int version;
	unsigned int frame_count;
	unsigned int frame_rate;
	unsigned int channel_count;
	unsigned int header_size;
	unsigned int block_size;
	unsigned int buffer_size;
	snd_asset_format format;
	snd_asset_channel channel_flags;
	snd_asset_flags flags;
	unsigned int seek_table_count;
	unsigned int *seek_table;
	unsigned int data_size;
	char *data;
};

struct snd_alias_t
{
	const char *name;
	unsigned int id;
	const char *subtitle;
	const char *secondaryname;
	struct SoundFile *soundFile;
	unsigned int flags;
	unsigned int duck;
	unsigned int contextType;
	unsigned int contextValue;
	unsigned __int16 fluxTime;
	unsigned __int16 startDelay;
	unsigned __int16 reverbSend;
	unsigned __int16 centerSend;
	unsigned __int16 volMin;
	unsigned __int16 volMax;
	unsigned __int16 teamVolMod;
	unsigned __int16 pitchMin;
	unsigned __int16 pitchMax;
	unsigned __int16 teamPitchMod;
	unsigned __int16 distMin;
	unsigned __int16 distMax;
	unsigned __int16 distReverbMax;
	unsigned __int16 envelopMin;
	unsigned __int16 envelopMax;
	unsigned __int16 envelopPercentage;
	char minPriorityThreshold;
	char maxPriorityThreshold;
	char probability;
	char occlusionLevel;
	char occlusionWetDry;
	char minPriority;
	char maxPriority;
	char pan;
	char dryCurve;
	char wetCurve;
	char dryMinCurve;
	char wetMinCurve;
	char limitCount;
	char entityLimitCount;
	char snapshotGroup;
};

struct snd_alias_list_t
{
	const char *name;
	unsigned int id;
	snd_alias_t *head;
	int count;
	int sequence;
};

struct LoadedSound
{
	const char *name;
	snd_asset sound;
};

struct PrimedSound
{
	const char *name;
	char *buffer;
	unsigned int size;
};

struct StreamedSound
{
	char *filename;
	PrimedSound *primeSnd;
};

union SoundFileRef
{
	LoadedSound *loadSnd;
	StreamedSound *streamSnd;
};

struct SoundFile
{
	SoundFileRef u;
	char type;
	char exists;
};

struct SndIndexEntry
{
	unsigned __int16 value;
	unsigned __int16 next;
};

struct SndBank
{
	const char *name;
	unsigned int aliasCount;
	snd_alias_list_t *alias;
	SndIndexEntry *aliasIndex;
	unsigned int packHash;
	unsigned int packLocation;
	unsigned int radverbCount;
	snd_radverb *radverbs;
	unsigned int snapshotCount;
	snd_snapshot *snapshots;
};
