#pragma once
#include "semver\semantic_version.h"

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

			if (constraint.GetMajorVersion() > version.GetMajorVersion())
				return false;

			if (constraint.GetMajorVersion() < version.GetMajorVersion())
				return true;

			if (constraint.GetPatchVersion() > version.GetPatchVersion())
				return false;

			return true;
		}
	}
}
