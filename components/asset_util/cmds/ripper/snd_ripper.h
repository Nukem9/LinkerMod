#pragma once
#include "process_info.h"
#include "snd_alias_db.h"
#include "snd_csv_enum.h"
#include "snd_driver_db.h"

#include <vector>

struct snd_ripper_instance_info_t
{
	FILE* outputFile;
	std::string& language;
	ForeignPointer<SndDriverGlobals>& globals;
	ForeignPointer<SndBank>& bank;

	std::vector<std::vector<ForeignPointer<snd_snapshot>>>* snapshots_table;
};

struct snd_csv_duck_entry_t
{
	unsigned int hash;
	const char* string;
};

double __cdecl SND_CSV_CENTS_Encode(double val);
double SND_CSV_CENTS_Decode(double val);
unsigned int __cdecl SND_HashName(const char *name);

void Snd_CSV_PrintHeader(FILE* f);

int Rip_Sound_GatherSnapshots_Callback_f(ForeignPointer<XAsset>& asset, ForeignPointer<XZoneName>& zoneName, void* data);
int Rip_Sound_Callback_f(ForeignPointer<XAsset>& asset, ForeignPointer<XZoneName>& zoneName, void* data);
