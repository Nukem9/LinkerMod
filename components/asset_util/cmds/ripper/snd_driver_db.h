#pragma once

enum snd_category_t
{
	SND_CATEGORY_SFX = 0x0,
	SND_CATEGORY_MUSIC = 0x1,
	SND_CATEGORY_VOICE = 0x2,
	SND_CATEGORY_UI = 0x3,
	SND_CATEGORY_COUNT = 0x4,
};

struct snd_group
{
	char name[32];
	char parentName[32];
	unsigned int id;
	int parentIndex;
	snd_category_t category;
	unsigned __int16 attenuationSp;
	unsigned __int16 attenuationMp;
};

struct snd_curve
{
	char name[32];
	unsigned int id;
	float points[8][2];
};

struct snd_pan
{
	char name[32];
	unsigned int id;
	float front;
	float back;
	float center;
	float lfe;
	float left;
	float right;
};

struct snd_snapshot_group
{
	char name[32];
};

struct snd_context
{
	unsigned int type;
	unsigned int valueCount;
	unsigned int values[8];
};

struct snd_master
{
	char name[32];
	unsigned int id;
	float notchE;
	float notchG;
	float notchF;
	float notchQ;
	float lowE;
	float lowG;
	float lowF;
	float lowQ;
	float peak1E;
	float peak1G;
	float peak1F;
	float peak1Q;
	float peak2E;
	float peak2G;
	float peak2F;
	float peak2Q;
	float hiE;
	float hiG;
	float hiF;
	float hiQ;
	float eqG;
	float compE;
	float compPG;
	float compMG;
	float compT;
	float compR;
	float compTA;
	float compTR;
	float limitE;
	float limitPG;
	float limitMG;
	float limitT;
	float limitR;
	float limitTA;
	float limitTR;
};

struct SndDriverGlobals
{
	const char *name;
	unsigned int groupCount;
	snd_group *groups;
	unsigned int curveCount;
	snd_curve *curves;
	unsigned int panCount;
	snd_pan *pans;
	unsigned int snapshotGroupCount;
	snd_snapshot_group *snapshotGroups;
	unsigned int contextCount;
	snd_context *contexts;
	unsigned int masterCount;
	snd_master *masters;
};
