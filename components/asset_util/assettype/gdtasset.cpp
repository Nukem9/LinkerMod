#include "gdtasset.h"
#include <regex>

const char* GDTAsset::gdf = "<error>.gdf";

GDTAsset::GDTAsset() : _name("<error>")
{
}

GDTAsset::~GDTAsset()
{
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
