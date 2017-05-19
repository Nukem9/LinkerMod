#include "stdafx.h"
#include <unordered_map>

/*
Note:
	stats arent saved when you die in a zm match (you have to do it via script)
	stats are? saved when you exit a map
	stats are saved & read when you close the game
	stats are saved & read when you switch mods

	NEEDS: to set up g_statsDir on loading a mod (or the game itself)
*/

static float default_vector[3] = { 0.0f, 0.0f, 0.0f };

UserStats_Value::UserStats_Value() : type(UserStats_ValueType::UNDEFINED)
{
	this->value.integer = 0;
}

UserStats_Value::UserStats_Value(UserStats_Value&& arg)
{
	this->type = arg.type;
	this->value.integer = arg.value.integer;
	arg.value.integer = 0;
}

UserStats_Value::UserStats_Value(const UserStats_Value& arg)
{
	*this = arg;
}

UserStats_Value::UserStats_Value(int _value) : type(UserStats_ValueType::INTEGER)
{
	this->value.integer = _value;
}

UserStats_Value::UserStats_Value(float _value) : type(UserStats_ValueType::DECIMAL)
{
	this->value.decimal = _value;
}

UserStats_Value::UserStats_Value(const std::string& _value) : type(UserStats_ValueType::STRING)
{
	this->value.string = new std::string(_value);
}

UserStats_Value::UserStats_Value(float* _value) : type(UserStats_ValueType::VECTOR)
{
	this->value.vector = new float[3];
	this->value.vector[0] = _value[0];
	this->value.vector[1] = _value[1];
	this->value.vector[2] = _value[2];
}

UserStats_Value::UserStats_Value(UserStats_ValueType type)
{
	this->type = type;
	switch (type)
	{
	case UserStats_ValueType::UNDEFINED:
	case UserStats_ValueType::INTEGER:
		this->value.integer = 0;
		break;
	case UserStats_ValueType::DECIMAL:
		this->value.decimal = 0.0f;
		break;
	case UserStats_ValueType::STRING:
		this->value.string = new std::string("");
		break;
	case UserStats_ValueType::VECTOR:
		this->value.vector = new float[3];
		value.vector[0] = 0.0f;
		value.vector[1] = 0.0f;
		value.vector[2] = 0.0f;
		break;
	default:
		ASSERT_MSG(0, "Unsupported UserStats_Value type");
	}
}

UserStats_Value::~UserStats_Value(void)
{
	if (type == UserStats_ValueType::STRING)
		delete this->value.string;
	else if (type == UserStats_ValueType::VECTOR)
		delete[] this->value.vector;

	this->value.integer = 0;
}

UserStats_Value& UserStats_Value::operator=(const UserStats_Value& arg)
{
	this->type = arg.type;
	switch (type)
	{
	case UserStats_ValueType::UNDEFINED:
	case UserStats_ValueType::INTEGER:
	case UserStats_ValueType::DECIMAL:
		this->value.integer = arg.value.integer;
		break;
	case UserStats_ValueType::STRING:
		if (arg.value.string)
			this->value.string = new std::string(*arg.value.string);
		else
			this->value.string = new std::string("");
		break;
	case UserStats_ValueType::VECTOR:
		this->value.vector = new float[3];
		value.vector[0] = arg.value.vector[0];
		value.vector[1] = arg.value.vector[1];
		value.vector[2] = arg.value.vector[2];
		break;
	default:
		ASSERT_MSG(0, "Unsupported UserStats_Value type");
	}
	return *this;
}

UserStats_ValueType UserStats_Value::Type(void) const
{
	return this->type;
}

void UserStats_Value::MorphType(UserStats_ValueType type)
{
	// Cleanup any current value
	this->~UserStats_Value();

	*this = UserStats_Value(type);
}

bool UserStats_Value::SetValue(int value, bool allowMorph)
{
	if (type != UserStats_ValueType::INTEGER)
	{
		if (!allowMorph)
			return false;
		this->MorphType(UserStats_ValueType::INTEGER);
	}

	this->value.integer = value;
	return true;
}

bool UserStats_Value::SetValue(float value, bool allowMorph)
{
	if (type != UserStats_ValueType::DECIMAL)
	{
		if (!allowMorph)
			return false;
		this->MorphType(UserStats_ValueType::DECIMAL);
	}

	this->value.decimal = value;
	return true;
}

bool UserStats_Value::SetValue(const std::string& value, bool allowMorph)
{
	if (type != UserStats_ValueType::STRING)
	{
		if (!allowMorph)
			return false;
		this->MorphType(UserStats_ValueType::STRING);
	}

	if (!this->value.string)
		this->value.string = new std::string(value);
	else
		*this->value.string = value;
	return true;
}

bool UserStats_Value::SetValue(float* value, bool allowMorph)
{
	if (type != UserStats_ValueType::VECTOR)
	{
		if (!allowMorph)
			return false;
		this->MorphType(UserStats_ValueType::VECTOR);
	}

	this->value.vector[0] = value[0];
	this->value.vector[1] = value[1];
	this->value.vector[2] = value[2];
	return true;
}

void* UserStats_Value::Data(void) const
{
	switch (type)
	{
	case UserStats_ValueType::INTEGER:
		return (void*)&this->value.integer;
	case UserStats_ValueType::DECIMAL:
		return (void*)&this->value.decimal;
	case UserStats_ValueType::STRING:
		if (!value.string)
			return "";
		return (void*)value.string->c_str();
	case UserStats_ValueType::VECTOR:
		if (!this->value.vector)
			return default_vector;
		return this->value.vector;
	case UserStats_ValueType::UNDEFINED:
			return "";
	default:
		ASSERT_MSG(0, "Unsupported UserStats_Value type");
		return "";
	}
}

size_t UserStats_Value::DataSize(void) const
{
	switch (type)
	{
	case UserStats_ValueType::INTEGER:
		return sizeof(int);
	case UserStats_ValueType::DECIMAL:
		return sizeof(float);
	case UserStats_ValueType::STRING:
		if (!value.string)
			return 1;
		return value.string->size() + 1;
	case UserStats_ValueType::VECTOR:
		return sizeof(float) * 3;
	default:
		ASSERT_MSG(0, "Unsupported UserStats_Value type");
	case UserStats_ValueType::UNDEFINED:
		return 0;
	}
}

UserStats_Value UserStats_Value::FromData(void* data)
{
	int* type = (int*)data;
	UserStats_Value stat((UserStats_ValueType)*type);
	if (stat.type == UserStats_ValueType::STRING)
	{
		*stat.value.string = (char*)data + 4;
		return stat;
	}
	else if (stat.type == UserStats_ValueType::VECTOR)
	{
		float* vec = ((float*)data) + 1;
		stat.value.vector[0] = vec[0];
		stat.value.vector[1] = vec[1];
		stat.value.vector[2] = vec[2];
		return stat;
	}

	stat.value.integer = ((int*)data)[1];
	return stat;
}

void UserStats_Value::EmitScriptValue(int _inst) const
{
	scriptInstance_t inst = (scriptInstance_t)_inst;

	switch (type)
	{
	case UserStats_ValueType::INTEGER:
		Scr_AddInt(value.integer, inst);
		return;
	case UserStats_ValueType::DECIMAL:
		Scr_AddFloat(value.decimal, inst);
		return;
	case UserStats_ValueType::STRING:
		if (!value.string)
		{
			Scr_AddUndefined(inst);
			return;
		}
		
		Scr_AddString(value.string->c_str(), inst);
		return;
	case UserStats_ValueType::VECTOR:
		if (value.vector != NULL)
		{
			Scr_AddVector(value.vector, inst);
			return;
		}
	case UserStats_ValueType::UNDEFINED:
	default:
		Scr_AddUndefined(inst);
		return;
	}
}

UserStats::UserStats()
{
}

UserStats::~UserStats()
{
}

void UserStats::Clear(void)
{
	map.clear();
}

const UserStats_Value& UserStats::GetStat(const std::string& key) const
{
	static UserStats_Value default_value;

	auto val = map.find(key);
	if (val == map.end())
		return default_value;

	return val->second;
}

bool UserStats::SetStat(const std::string& key, int value)
{
	map[key] = UserStats_Value(value);
	return true;
}
bool UserStats::SetStat(const std::string& key, float value)
{
	map[key] = UserStats_Value(value);
	return true;
}

bool UserStats::SetStat(const std::string& key, const std::string& value)
{ 
	map[key] = UserStats_Value(value);
	return true;
}

bool UserStats::SetStat(const std::string& key, float* value)
{
	map[key] = UserStats_Value(value);
	return true;
}

int UserStats::ReadFile(const char* filename)
{
	this->Clear();

	std::string path = va("%s\\players\\%s", fs_homepath->current.string, filename);

	FILE* h = NULL;
	fopen_s(&h, path.c_str(), "rb");
	fseek(h, 0, SEEK_END);
	unsigned int size = ftell(h);
	fseek(h, 0, SEEK_SET);

	BYTE* buf = new BYTE[size + 1];
	fread(buf, 1, size, h);

	buf[size] = 0; // Ensure that the last byte in the buffer is null to prevent unterminated strings

	fclose(h);

	for (BYTE* ptr = buf; (unsigned int)(ptr - buf) < size;)
	{
		std::string name = (char*)(ptr);
		if (name[0] == '\0')
			break;

		ptr += name.size() + 1;

		if ((unsigned int)(ptr - buf) >= size)
			break;

		UserStats_Value stat = UserStats_Value::FromData((void*)ptr);
		ptr += stat.DataSize() + 4; // append the pointer by the size of the stat entry + the size of the type specifier

		map[name] = stat;
		
		Com_Printf(0, "READ STAT: %s\n", name.c_str());
	}

	return 0;
}

int UserStats::WriteFile(const char* filename) const
{
	int h = FS_FOpenFileWriteToDir(filename, "players", fs_homepath->current.string);
	for (auto stat : this->map)
	{
		auto key = stat.first;
		auto val = stat.second;

		int type = (int)val.Type();

		FS_Write(key.c_str(), key.size() + 1, h);
		FS_Write(&type, sizeof(type), h);

		FS_Write(val.Data(), val.DataSize(), h);

	}

	FS_FCloseFile(h);
	return 0;
}

typedef void(__cdecl* LiveStorage_ReadStatsFromDir_t)(const char* directory);
LiveStorage_ReadStatsFromDir_t LiveStorage_ReadStatsFromDir_o = (LiveStorage_ReadStatsFromDir_t)0x00431400;

typedef void(__cdecl* LiveStorage_Init_t)(void);
LiveStorage_Init_t LiveStorage_Init_o = (LiveStorage_Init_t)0x004E14F0;

//
// Ensure that the mod's respective stats are loaded
// when the user switches mods
//
void __declspec(naked) mfh_CL_Disconnect(void)
{
	static void* dwRetn = (void*)0x005FEE51;
	_asm
	{
		jz RETURN
		call LiveStorage_UploadStats
		call UserStorage_ReadStatsIfDirChanged
RETURN:
		jmp dwRetn
	}
}

void LiveStorage_InitMod(void)
{
	LiveStorage_Init_o = (LiveStorage_Init_t)Detours::X86::DetourFunction((PBYTE)0x004E14F0, (PBYTE)&LiveStorage_Init);

	LiveStorage_ReadStatsFromDir_o = (LiveStorage_ReadStatsFromDir_t)Detours::X86::DetourFunction((PBYTE)0x00431400, (PBYTE)&LiveStorage_ReadStatsFromDir);
	Detours::X86::DetourFunction((PBYTE)0x00503FB0, (PBYTE)&LiveStorage_UploadStats);
	Detours::X86::DetourFunction((PBYTE)0x005FEE4A, (PBYTE)&mfh_CL_Disconnect);
}

void LiveStorage_Init(void)
{
	LiveStorage_Init_o();
	UserStorage_Init();
}

void LiveStorage_ReadStatsFromDir(const char* directory)
{
	LiveStorage_ReadStatsFromDir_o(directory);
	UserStorage_ReadStatsFromDir(directory);
}

// Called in mfh_CL_Disconnect
// Provides additional behavior only
void UserStorage_ReadStatsIfDirChanged()
{
	if (G_ExitAfterToolComplete())
		return;

	if (_stricmp((const char*)g_statsDir, fs_gameDirVar->current.string)) // todo - add this check
		UserStorage_ReadStatsFromDir(fs_gameDirVar->current.string);
	else
		Com_Printf(0, "Stats Dir hasnt changed");
}

void LiveStorage_UploadStats(void)
{
	if (G_ExitAfterToolComplete())
		return;

	LiveStorage_WriteBasicStats(0);
	UserStorage_UploadStats();
}

//
// Offline Stats
//

UserStats g_userStats;// = NULL;

void UserStorage_Init(void)
{
	//if (g_userStats)
	//{
	//	Com_Error(0, "g_userStats has already been initialized!\n");
	//	return;
	//}

	g_userStats.Clear();
	g_userStats.ReadFile("stats.bin");

	//g_userStats = new UserStats;
	//sprintf_s(g_statsDir, 256, fs_gameDirVar->current.string);
}

void UserStorage_Free(void)
{
	//if (g_userStats)
	//	delete g_userStats;
	g_userStats.Clear();

	g_statsDir[0] = '\0';
}

void UserStorage_ReadStatsFromDir(const char* directory)
{
	//if (!g_userStats)
	//{
	//	Com_Error(0, "Stats arent availalble\n");
	//	return;
	//}

	Com_Printf(0, "LiveStorage_ReadStatsFromDir\n");
	g_userStats.ReadFile("stats.bin");
}

void UserStorage_UploadStats(void)
{
	//if (!g_userStats)
	//{
	//	Com_Error(0, "Tried to write stats before they were intialized\n");
	//	return;
	//}

	Com_Printf(0, "LiveStorage_UploadStats\n");
	g_userStats.WriteFile("stats.bin");
}
