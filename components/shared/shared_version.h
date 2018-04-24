#pragma once

#include <sstream>
#include <string>
#include "semver/semantic_version.h"

typedef semver::v2::Version Version_t;

#ifdef G_VERSION
	const Version_t g_version(G_VERSION);
	
	__declspec(dllexport) const Version_t& DLL_Version()
	{
		return g_version;
	}
	
	__declspec(dllexport) const char* DLL_VersionString()
	{
		static char version[32] = "";

		if (*version == '\0')
		{
			std::ostringstream oss;
			oss << g_version;

			ASSERT(oss.str().length() < ARRAYSIZE(version) - 1);
			strcpy_s(version, ARRAYSIZE(version), oss.str().c_str());
		}
	
		return version;
	}
#else
__declspec(dllexport) const Version_t& DLL_Version();
__declspec(dllexport) const char* DLL_VersionString();
#endif

//
// The library's comparison operators are screwed up - so we need a custom comparison func
//
namespace semver
{
	namespace v2
	{
		static bool IsCompatibleVersion(Version& constraint, Version& version)
		{
			if (constraint.GetMajorVersion() != version.GetMajorVersion())
				return false;

			if (constraint.GetMinorVersion() > version.GetMinorVersion())
				return false;

			if (constraint.GetMinorVersion() < version.GetMinorVersion())
				return true;

			if (constraint.GetPatchVersion() > version.GetPatchVersion())
				return false;

			return true;
		}
	}
}
