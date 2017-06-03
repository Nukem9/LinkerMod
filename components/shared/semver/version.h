#pragma once

#include <sstream>
#include "semantic_version.h"

typedef semver::v2::Version Version_t;

#ifdef G_VERSION
	const Version_t g_version(G_VERSION);
	
	__declspec(dllexport) const Version_t& DLL_Version()
	{
		return g_version;
	}
	
	__declspec(dllexport) const std::string DLL_VersionString()
	{
		std::ostringstream oss;
		oss << g_version;
		return oss.str();
	}
#else
__declspec(dllexport) const Version_t& DLL_Version();
__declspec(dllexport) const std::string DLL_VersionString();
#endif


