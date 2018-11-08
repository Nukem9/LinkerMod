#include "stdafx.h"

#include "pe.h"

#include <stdarg.h>
#include <fstream>
#include <cctype>

#include <functional>

using namespace pe_bliss;

//
// Utility functions
//

template<class _Elem,
	class _Traits = std::char_traits<_Elem>,
	class _Alloc = std::allocator<_Elem>>
static std::basic_string<_Elem, _Traits, _Alloc> str_toupper(std::basic_string<_Elem, _Traits, _Alloc> str) {
	std::transform(str.begin(), str.end(), str.begin(),
		[](_Elem c) { return std::toupper(c); }
	);
	return str;
}

static std::string str_toupper(const char* pStr) {
	std::string str = pStr;
	std::transform(str.begin(), str.end(), str.begin(),
		[](std::string::value_type c) { return std::toupper(c); }
	);
	return str;
}

static std::string va(const char* fmt, ...) {
	std::string result(128, '\0');
	va_list args;
	va_start(args, fmt);
	size_t len = vsnprintf((char*)result.data(), 0, fmt, args);
	va_end(args);

	if (len < 0)
		throw "Invalid string";

	result.resize(len + 1);

	va_start(args, fmt);
	len = vsnprintf((char*)result.data(), len + 1, fmt, args);
	va_end(args);

	if (len < 0)
		throw "Unable to generate string";

	return result;
}

namespace pe {
	//
	// Automatic handler for reading PE files
	// 
	template<typename ReturnType>
	static ReturnType HandleFile(const std::string& filepath, std::function<ReturnType(pe_base& image)> handler) {
		std::ifstream pe_file(filepath, std::ios::in | std::ios::binary);
		if (!pe_file) {
			throw "Could not open file";
			return (ReturnType)-1;
		}

		pe_base image(pe_factory::create_pe(pe_file));
		pe_file.close();
		
		return handler(image);
	}

	//
	// Check if a given image imports a given function from the specified module
	// If symbolName is null, it only checks if the module is imported by the image
	//
	bool HasImport(const pe_base& image, const char* moduleName, const char* symbolName, bool caseSensitive) {
		// Automatically format names depending on whether or not we're in case sensitive mode
		const auto FormatName = [caseSensitive](const std::string str) {
			if (caseSensitive)
				return str;
			return str_toupper(str);
		};

		const auto _module = FormatName(moduleName);
		const auto _symbol = symbolName != nullptr ? FormatName(symbolName) : std::string("");

		// Iterate over all modules used by the given image
		const auto moduleList = get_imported_functions(image);
		for (const auto& library : moduleList) {
			const auto libraryName = FormatName(library.get_name());

			// Skip all modules except the one we're asking about
			if (libraryName != _module) {
				continue;
			}

			// If the user didnt provide a symbol name we just return true
			if (symbolName == nullptr) {
				return true;
			}

			// Iterate over all functions imported from the given module
			const auto& functionList = library.get_imported_functions();
			for (const auto& function : functionList) {
				const auto functionName = FormatName(function.get_name());
				// Skip all modules except the one we're talking about
				if (functionName != _symbol) {
					continue;
				}

				return true;
			}
		}

		return false;
	}

	bool HasModule(const pe_base& image, const char* moduleName, bool caseSensitive) {
		return HasImport(image, moduleName, nullptr, caseSensitive);
	}

	bool HasModule(const char* filepath, const char* moduleName, bool caseSensitive) {
		return HandleFile<bool>(filepath, [=](pe_base& image) {
			return HasModule(image, moduleName, caseSensitive);
		});
	}

	bool HasImport(const char* filepath, const char* moduleName, const char* symbolName, bool caseSensitive) {
		return HandleFile<bool>(filepath, [=](pe_base& image) {
			return HasImport(image, moduleName, symbolName, caseSensitive);
		});
	}

	//
	// Attempt to add an import to the PE at the given filepath
	//
	int AddImport(const char* filepath, const char* moduleName, const char* symbol) {
		return HandleFile<int>(filepath, [=](pe_base& image) {
			auto booleo = HasImport(image, moduleName, symbol, false);

			std::string s = va("%s", booleo ? " ture" : "Faflas");

			MessageBoxA(0, s.c_str(), s.data(), 0);
			return 0;
		});
	}
}
