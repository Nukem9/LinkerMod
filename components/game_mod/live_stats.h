#pragma once

#include <unordered_map>
#include <string>

enum class UserStats_ValueType
{
	UNDEFINED = 0,

	INTEGER,
	DECIMAL,
	STRING,
	VECTOR,

};

struct UserStats_Value
{
private:
	UserStats_ValueType type;

	union
	{
		int integer;
		float decimal;
		std::string* string;
		float* vector;
	} value;

public:
	UserStats_Value(void);
	UserStats_Value(UserStats_Value&&);
	UserStats_Value(const UserStats_Value&);

	UserStats_Value(int value);
	UserStats_Value(float value);
	UserStats_Value(const std::string& value);
	UserStats_Value(float* value);

	UserStats_Value(UserStats_ValueType type);

	~UserStats_Value(void);

	UserStats_Value& operator=(const UserStats_Value&);

	UserStats_ValueType Type(void) const;
	void MorphType(UserStats_ValueType type);

	bool SetValue(int value, bool allowMorph = false);
	bool SetValue(float value, bool allowMorph = false);
	bool SetValue(const std::string& value, bool allowMorph = false);
	bool SetValue(float* value, bool allowMorph = false);

	void* Data(void) const;
	size_t DataSize(void) const;

	static UserStats_Value FromData(void* data);

	void EmitScriptValue(int inst) const;
};

class UserStats
{
private:
	std::unordered_map <std::string, UserStats_Value> map;

public:
	UserStats(void);
	~UserStats(void);

	void Clear();

	const UserStats_Value& GetStat(const std::string& key) const;

	bool SetStat(const std::string& key, int value);
	bool SetStat(const std::string& key, float value);
	bool SetStat(const std::string& key, const std::string& value);
	bool SetStat(const std::string& key, float* value);

	int ReadFile(const char* filename);
	int WriteFile(const char* filename) const;
};

extern UserStats g_userStats;

typedef struct TaskRecord *(__cdecl* LiveStorage_WriteBasicStats_t)(const int controllerIndex);
static LiveStorage_WriteBasicStats_t LiveStorage_WriteBasicStats = (LiveStorage_WriteBasicStats_t)0x0054D720;

static char* g_statsDir = (char*)0x0384DCA8;

void LiveStorage_InitMod(void);

void LiveStorage_Init(void);
void LiveStorage_ReadStatsFromDir(const char* directory);
void LiveStorage_UploadStats(void);

void UserStorage_Init(void);
void UserStorage_Free(void);
void UserStorage_ReadStatsFromDir(const char* directory);
void UserStorage_ReadStatsIfDirChanged();
void UserStorage_UploadStats(void);
