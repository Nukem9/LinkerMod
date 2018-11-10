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

	std::string NormalizePath(const std::string& path)
	{
		std::string out = "";
		out.reserve(path.size() + 1);

		char prev = '\0';
		for (char c : path)
		{
			if (c == '/' || c == '\\')
			{
				c = '\\';
				if (c == prev)
				{
					continue;
				}
			}

			out.push_back(prev = c);
		}

		// Remove trailing delimiters
		out.erase(std::find_if(out.rbegin(), out.rend(), [](int ch) {
			return ch != '\\';
		}).base(), out.end());

		return out;
	}

	// TODO: Properly check this
	std::string JoinPath(const std::string& path1, const std::string& path2)
	{
		return NormalizePath(path1 + "\\" + path2);
	}
};
