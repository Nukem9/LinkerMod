#pragma once

#include <pe_lib/pe_bliss.h>

namespace pe {
	int AddImport(const char* filepath, const char* moduleName, const char* symbol);
	
	bool HasModule(const char* filepath, const char* moduleName, bool caseSensitive = false);
	bool HasImport(const char* filepath, const char* moduleName, const char* symbolName, bool caseSensitive = false);

	bool HasModule(const pe_bliss::pe_base& image, const char* moduleName, bool caseSensitive = false);
	bool HasImport(const pe_bliss::pe_base& image, const char* moduleName, const char* symbolName, bool caseSensitive = false);
}
