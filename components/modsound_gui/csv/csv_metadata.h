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

	//union
	//{
	//	const char* _str;
	//	int _enum;
	//	int _int;
	//	float _float;
	//} _default;

	const char* enumTable;
	const char* enumColumn;

	const char* unit;
	const char* displayUnit;

public:
	csv_metadata_s(void);
	~csv_metadata_s(void);
};

void CSV_Metadata_Init(void);
