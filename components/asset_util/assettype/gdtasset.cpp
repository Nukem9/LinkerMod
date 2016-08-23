#include "gdtasset.h"
#include <regex>

const char* GDTAsset::gdf = "<error>.gdf";

GDTAsset::GDTAsset() : _name("<error>")
{
}

GDTAsset::~GDTAsset()
{
}

std::string ExtractStringProperty(std::string prop, std::string _default, std::string src)
{
	std::string out = _default;

	char buf[512] = "";
	sprintf_s(buf, "%s%s%s", "self\\.", prop.c_str(), ".*?\\\"(\\w*)\\\";");
	std::regex ex(buf);// \\\"(\\w+)\\\";");
	std::smatch sm;

	int modelCount = 0;
	while (std::regex_search(src, sm, ex))
	{
		if (sm.size() == 2)
			return sm[1];

		src = sm.suffix().str();
	}

	return out;
}

std::string ExtractAliasProperty(std::string prop, std::string _default, std::string src)
{
	std::string out = _default;

	char buf[512] = "";
	sprintf_s(buf, "%s%s%s", "self\\.", prop.c_str(), ".*?\\\\(\\w+)::main.*?;");
	std::regex ex(buf);// \\\"(\\w+)\\\";");
	std::smatch sm;

	int modelCount = 0;
	while (std::regex_search(src, sm, ex))
	{
		if (sm.size() == 2)
			return sm[1];

		src = sm.suffix().str();
	}

	return out;
}

std::string ExtractRadiantModelProperty(std::string src)
{
	std::string out = "";

	char buf[512] = "";

	std::regex ex("defaultmdl=\\\"(\\w+)\\\".*");
	std::smatch sm;

	int modelCount = 0;
	while (std::regex_search(src, sm, ex))
	{
		if (sm.size() == 2)
			return sm[1];

		src = sm.suffix().str();
	}

	return out;
}

void ExtractCharacterModelProperties(std::string _default, std::string src, std::string* dst, int maxCount)
{
	_ASSERT(dst != NULL);

	std::string out = _default;

	std::regex ex("character\\\\(\\w+)::main");
	std::smatch sm;

	int modelCount = 0;
	while (std::regex_search(src, sm, ex))
	{
		if (sm.size() == 2)
		{
			dst[modelCount++] = sm[1];
			if (modelCount >= maxCount)
			{
				printf("Too many model entries\n");
				return;
			}
		}

		src = sm.suffix().str();
	}
}


std::string ExtractEngagementProperty(std::string prop, std::string _default, std::string src, int arg)
{
	std::string out = _default;

	char buf[512] = "";
	sprintf_s(buf, "%s%s%s", "self\\s+", prop.c_str(), "\\(\\s*(\\d+\\.?\\d+)\\s*,\\s*(\\d+\\.?\\d+)\\s*\\);");
	std::regex ex(buf);
	std::smatch sm;

	int modelCount = 0;
	while (std::regex_search(src, sm, ex))
	{
		if (sm.size() == 3)
			return sm[arg + 1];

		src = sm.suffix().str();
	}

	return out;
}

std::string ExtractNumberProperty(std::string prop, std::string _default, std::string src)
{
	std::string out = _default;

	char buf[512] = "";
	sprintf_s(buf, "%s%s%s", "self\\.", prop.c_str(), ".*?(\\d+\\.?\\d+);");
	std::regex ex(buf);// \\\"(\\w+)\\\";");
	std::smatch sm;

	int modelCount = 0;
	while (std::regex_search(src, sm, ex))
	{
		if (sm.size() == 2)
			return sm[1];

		src = sm.suffix().str();
	}

	return out;
}
