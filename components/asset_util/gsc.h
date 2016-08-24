#pragma once
#include <string>
#include "assettype\character.h"

//
// Extract the value of a variable that is defined using the following format(s):
// var="Value"
// var = "Value"
//
int GSC_ExtractStringVariable(const char* var, const char* src, std::string* dst, const char* defaultValue="");

//
// Extract properties that use the format:
//	self.<prop> = "Value";
//
int GSC_ExtractStringProperty(const char* prop, const char* src, std::string* dst, const char* defaultValue="");

//
// Extract alias properties from lines that use the format:
//	self.<prop> = ************alias::main******;
//
int GSC_ExtractAliasProperty(const char* prop, const char* src, std::string* dst, const char* defaultValue);

//
// Extract properties that use the format:
//	self.<prop> = 1.0;
//
int GSC_ExtractNumberProperty(const char* prop, const char* src, std::string* dst, const char* defaultValue);


//
// Extract the character names from lines with the format:
//	character\<character name>::main(...);
// Returns the number of matches found
//
int GSC_AIType_ExtractCharacters(std::string src, std::string* dst, int maxCount, const char* defaultValue="");

enum AI_ENGAGEMENTTYPE
{
	AI_ENGAGEMENTTYPE_DIST = 0,
	AI_ENGAGEMENTTYPE_FALLOFFDIST = 1,
};

//
// Extract the engagement properties from lines with the format:
//	self <prop>(dist, falloffdist);
// engagementType controls which of the arguments is used
//
int GSC_AIType_ExtractEngagementProperty(const char* prop, AI_ENGAGEMENTTYPE engagementType, const char* src, std::string* dst, const char* defaultValue);

//
// Extract alias / string combos from a source string
//
int GSC_Character_ExtractAliasEntry(const char* prop, const char* src, aliasEntry_s* dst, const char* defaultValue);

//
// Extract gibSpawn / alias combos from a source string
//
int GSC_Character_ExtractGibSpawnEntries(const char* src, gibSpawn_s* dst, int maxCount, const char* defaultValue);