#pragma once
#include <fstream>

class GDTAsset
{
protected:
	static const char* gdf;
	std::string _name;

public:
	GDTAsset();
	~GDTAsset();

	virtual int Write(FILE* f) = 0;
};

std::string ExtractStringProperty(std::string prop, std::string _default, std::string src);
std::string ExtractAliasProperty(std::string prop, std::string _default, std::string src);
std::string ExtractRadiantModelProperty(std::string src);
void ExtractCharacterModelProperties(std::string _default, std::string src, std::string* dst, int maxCount);
std::string ExtractEngagementProperty(std::string prop, std::string _default, std::string src, int arg);
std::string ExtractNumberProperty(std::string prop, std::string _default, std::string src);
