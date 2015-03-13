#pragma once
#include "db_registry.h"

struct s_dumpSettings
{
	bool dumpTechsets;
	bool dumpTechniques;
	bool dumpShaders;
	bool skipDuplicates;
	bool overwriteFiles;
};

int DumpAssets(s_dumpSettings dumpSettings); 
//int DumpTechset(const char* path, XAsset* asset);
//int DumpTechnique(const char* path, MaterialTechnique* technique);
//int DumpVertexShader(const char* path, MaterialVertexShader* vs);
//int DumpPixelShader(const char* path, MaterialPixelShader* ps);