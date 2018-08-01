#pragma once
#include "stdafx.h"
#include "fs.h"

namespace FS
{
	bool FileExists(const std::string& filepath)
	{		
		WIN32_FIND_DATAA findData;
		HANDLE h = FindFirstFileA(filepath.c_str(), &findData);

		if (h == INVALID_HANDLE_VALUE)
			return false;

		FindClose(h);
		
		return true;
	}

	// TODO: Properly check this
	std::string JoinPath(const std::string& path1, const std::string& path2)
	{
		return path1 + "\\" + path2;
	}
};
