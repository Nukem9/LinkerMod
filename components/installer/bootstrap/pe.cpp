#include "stdafx.h"

#include "pe.h"

#include <stdarg.h>
#include <fstream>
#include <cctype>

#include <functional>

using namespace pe_bliss;

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
		const auto _symbol = symbolName != nullptr ? FormatName(symbolName) : std::string();

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
	int AddImport(const char* filepath, const char* moduleName, const char* symbolName) {
		return HandleFile<int>(filepath, [=](pe_base& image) {
			// Don't bother if the import is already present on the binary
			if(HasImport(image, moduleName, symbolName, false)) {
				return 0;
			}

			// Get a list of the imported libraries & functions
			auto libraries = get_imported_functions(image);

			// Create a new library entry which will import the user specified function
			import_library userLib;
			userLib.set_name(moduleName); // Use the user specified module

			// Create an import entry for the user specified function
			imported_function userFunc;
			userFunc.set_name(symbolName);	// Use the user defined symbol name
			userFunc.set_iat_va(0x0);		// Write a non-zero absolute address in the import address table ??????????

			// Add the function to the new library & the new library to the imported library list
			userLib.add_import(userFunc);	// Actually add the function to the library
			libraries.push_back(userLib);	// Actually add the library to the image's imports

			// You can edit existing imports

			// But we'll just rebuild the import table
			// It will be larger than before our editing
			// so we write it in a new section so that everything fits
			// (we cannot expand existing sections, unless the section is right at the end of the file)
			section newImports;
			newImports.set_name(".hack");	// Assign a name to the given section
			newImports.readable(true);		// Enable reading
			newImports.writeable(true);		// Enable writing
			newImports.get_raw_data().resize(0x1);	// Empty sections are not allowed
													// so we need to ensure the buffer has a size of at least 1

			// Add the new imports section to the image
			// A reference to the added section with calculated dimensions is returned
			section& attachedSection = image.add_section(newImports);

			// Setup the imports rebuilder & rebuild the imports for the image
			import_rebuilder_settings settings(true, false); // Modify the PE header but do not clear the IMAGE_DIRECTORY_ENTRY_IAT field
			rebuild_imports(image, libraries, attachedSection, settings);

			// Open the existing binary for writing, and rebuild the PE file
			const auto mode =	std::ios::out |		// Open with write access
								std::ios::binary |	// Open in binary mode
								std::ios::trunc;	// Clear any existing file contents
			std::ofstream outFile(filepath, std::ios::out | std::ios::binary | std::ios::trunc);
			if (!outFile) {
				throw "Could not open file for writing!";
				return -1;
			}

			// Rebuild the PE file & write the data to the output file
			rebuild_pe(image, outFile);
			outFile.close();

			return 0;
		});
	}
}
