#pragma once
#include <string>
#include <unordered_map>

//extern std::unordered_map<CSVMetadata*, std::string>> g_metadata_map;

enum class CSV_WIGET_TYPE
{
	CSV_WIDGET_TEXT,
	CSV_WIDGET_COMBO,
	CSV_WIDGET_KNOB,
	CSV_WIDGET_NUMBER,
	CSV_WIDGET_CHECKBOX,
	CSV_WIDGET_CHECKLIST,

	CSV_WIDGET_COUNT
};

enum class CSV_FIELD_TYPE
{
	CSV_FIELD_BOOL,
	CSV_FIELD_INT,
	CSV_FIELD_FLOAT,
	CSV_FIELD_STRING,
	CSV_FIELD_ENUM,
	CSV_FIELD_FLAGS,
};

class csv_metadata_s
{
public:
	const char* table;
	const char* column;
	const char* title;
	const char* tip;
	CSV_WIGET_TYPE widget;
	CSV_FIELD_TYPE type;

	float _min;
	float _max;

	const char* default_str;

	union {
		int integer_value;
		float float_value;
		bool boolean_value;
		int enum_index;
	} _default;

	const char* enumTable;
	const char* enumColumn;

	const char* unit;
	const char* displayUnit;

public:
	csv_metadata_s(void);
	~csv_metadata_s(void);
};

class CSV_Metadata_Globals
{
public:
	std::vector<csv_metadata_s> metadata;
	
	struct
	{
		const char** pans;
		const char** curves;
		const char** snapshots;
		const char** randomize_type;
	} enums;

	CSV_Metadata_Globals(void);
	~CSV_Metadata_Globals(void);
};

extern const char* csv_enum_bus[4];
extern const char* csv_enum_looping[3];
extern const char* csv_enum_priority[5];
extern const char* csv_enum_move_type[9];
extern const char* csv_enum_spatialized[4];
extern const char* csv_enum_storage_type[5];
extern const char* csv_enum_yes_no[3];
extern const char* csv_enum_yes_no_both[4];
extern const char* csv_enum_pc_format[2];

extern CSV_Metadata_Globals g_metadata;

int CSV_Metadata_Init(void);
