#pragma once
#include <string>

namespace FS
{
	bool FileExists(const std::string& filepath);
	std::string JoinPath(const std::string& path1, const std::string& path2);
};
