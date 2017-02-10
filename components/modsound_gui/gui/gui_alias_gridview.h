#pragma once
#include "../csv/csv.h"
#include "../csv/csv_alias.h"

struct GUIAliasGridView_AliasFieldDef
{
	int data_offset;
	csv_metadata_s* metadata;
	void(*draw_func)(const char* label, void* data, void* metadata, float width);
};

class GUIAliasGridView
{
private:
	std::vector<csv_snd_alias_t> aliases;
	std::vector<GUIAliasGridView_AliasFieldDef> field_defs;

	//void LoadFieldDef(const char* field)

public:
	GUIAliasGridView(void);
	~GUIAliasGridView(void);
	
	int LoadTable(void); // Load the test table
	int LoadTable(const char* path);

	void Draw(void);
};
