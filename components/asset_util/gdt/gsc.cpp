#include "../shared/utility.h"
#include "gsc.h"
#include "assettype\character.h"
#include <regex>

int GSC_ExtractStringVariable(const char* var, const char* src, std::string* dst, const char* defaultValue)
{
	DBG_ASSERT(var != NULL);
	DBG_ASSERT(src != NULL);
	DBG_ASSERT(dst != NULL);
	DBG_ASSERT(defaultValue != NULL);

	char buf[512] = "";
	sprintf_s(buf, "%s%s", var, "\\s*=\\s*\\\"(\\w+)\\\".*");

	std::regex ex(buf);
	std::cmatch sm;

	if (std::regex_search(src, sm, ex))
	{
		if (sm.size() == 2)
		{
			*dst = sm[1];
			return 0;
		}
	}

	*dst = defaultValue;
	return sm.size();
}

int GSC_ExtractStringProperty(const char* prop, const char* src, std::string* dst, const char* defaultValue)
{
	DBG_ASSERT(prop != NULL);
	DBG_ASSERT(src != NULL);
	DBG_ASSERT(dst != NULL);
	DBG_ASSERT(defaultValue != NULL);

	char buf[512] = "";
	sprintf_s(buf, "%s%s%s", "self\\.", prop, ".*?\\\"(\\w*)\\\";");
	
	std::regex ex(buf);
	std::cmatch sm;

	if (std::regex_search(src, sm, ex))
	{
		if (sm.size() == 2)
		{
			*dst = sm[1];
			return 0;
		}
	}

	*dst = defaultValue;
	return sm.size();
}

int GSC_ExtractAliasProperty(const char* prop, const char* src, std::string* dst, const char* defaultValue)
{
	DBG_ASSERT(prop != NULL);
	DBG_ASSERT(src != NULL);
	DBG_ASSERT(dst != NULL);
	DBG_ASSERT(defaultValue != NULL);

	char buf[512] = "";
	sprintf_s(buf, "%s%s%s", "self\\.", prop, ".*?\\\\(\\w+)::main.*?;");

	std::regex ex(buf);
	std::cmatch sm;

	if (std::regex_search(src, sm, ex))
	{
		if (sm.size() == 2)
		{
			*dst = sm[1];
			return 0;
		}
	}

	*dst = defaultValue;
	return sm.size();
}

int GSC_ExtractNumberProperty(const char* prop, const char* src, std::string* dst, const char* defaultValue)
{
	DBG_ASSERT(prop != NULL);
	DBG_ASSERT(src != NULL);
	DBG_ASSERT(dst != NULL);
	DBG_ASSERT(defaultValue != NULL);

	char buf[512] = "";
	sprintf_s(buf, "%s%s%s", "self\\.", prop, ".*?=.*?(\\d+(?:\\.?\\d+)?);");

	std::regex ex(buf);
	std::cmatch sm;

	if (std::regex_search(src, sm, ex))
	{
		if (sm.size() == 2)
		{
			*dst = sm[1];
			return 0;
		}
	}

	*dst = defaultValue;
	return sm.size();
}


//
// AIType Specific
//

int GSC_AIType_ExtractCharacters(std::string src, std::string* dst, int maxCount, const char* defaultValue)
{
	DBG_ASSERT(dst != NULL);
	DBG_ASSERT(defaultValue != NULL);

	std::regex ex("character\\\\(\\w+)::main");
	std::cmatch sm;

	int matchCount = 0;
	while (std::regex_search(src.c_str(), sm, ex))
	{
		if (sm.size() == 2)
		{
			dst[matchCount++] = sm[1];

			if (matchCount > maxCount)
			{
				printf("Too many model entries\n");
				return matchCount;
			}
		}

		src = sm.suffix().str();
	}

	for (int i = matchCount; i < maxCount; i++)
	{
		dst[i] = defaultValue;
	}

	return matchCount;
}

int GSC_AIType_ExtractEngagementProperty(const char* prop, AI_ENGAGEMENTTYPE engagementType, const char* src, std::string* dst, const char* defaultValue)
{
	DBG_ASSERT(prop != NULL);
	DBG_ASSERT(src != NULL);
	DBG_ASSERT(dst != NULL);
	DBG_ASSERT(defaultValue != NULL);

	DBG_ASSERT(engagementType == AI_ENGAGEMENTTYPE_DIST || engagementType == AI_ENGAGEMENTTYPE_FALLOFFDIST);

	char buf[512] = "";
	sprintf_s(buf, "%s%s%s", "self\\s+", prop, "\\(\\s*(\\d+\\.?\\d+)\\s*,\\s*(\\d+\\.?\\d+)\\s*\\);");

	std::regex ex(buf);
	std::cmatch sm;

	if (std::regex_search(src, sm, ex))
	{
		if (sm.size() == 3)
		{
			*dst = sm[engagementType + 1];
			return 0;
		}
	}

	*dst = defaultValue;
	return sm.size();
}


//
// Character Specific
//
int GSC_Character_ExtractAliasEntry(const char* prop, const char* src, aliasEntry_s* dst, const char* defaultValue)
{
	DBG_ASSERT(prop != NULL);
	DBG_ASSERT(src != NULL);
	DBG_ASSERT(dst != NULL);

	DBG_ASSERT(defaultValue != NULL);

	GSC_ExtractStringProperty(prop, src, &dst->model, defaultValue);
	GSC_ExtractAliasProperty(prop, src, &dst->alias, defaultValue);

	return 0;
}

int GSC_Character_ExtractGibSpawnEntries(const char* src, gibSpawn_s* dst, int maxCount, const char* defaultValue)
{
	DBG_ASSERT(dst != NULL);
	DBG_ASSERT(defaultValue != NULL);

	for (int i = 0; i < maxCount; i++)
	{
		char buf[32] = "";
		sprintf_s(buf, "gibSpawn%d", i+1);
		GSC_Character_ExtractAliasEntry(buf, src, &dst[i].alias, "");

		sprintf_s(buf, "gibSpawnTag%d", i+1);
		GSC_ExtractStringProperty(buf, src, &dst[i].tag, "");
	}

	return 0;
}
