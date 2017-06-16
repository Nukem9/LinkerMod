#include "../sys/AppInfo.h"
#include "../common/io.h"
#include "../common/fs.h"

#include "../cvar.h"
#include "cmd_common.h"

#include "../gsc_parser/gsc_parser.h"
#pragma comment(lib, "gsc_parser.lib")

#include <set>
#include <functional>

//
// Enumerate over the children of a symbol, cancels if the callback returns false
//
void Symbol_EnumChildren(Symbol* symbol, std::function<bool(Symbol *)> callback)
{
	for (Symbol* c = symbol->Children(); c; c = c->NextElem())
	{
		if (!callback(c))
		{
			return;
		}
	}
}

//
// Get the main function from the global AST group
//
Function* AST_FindFunction(Symbol* AST, const char* name)
{
	for (Symbol* c = AST->Children(); c; c = c->NextElem())
	{
		if (c->Type() == S_TYPE_FUNCTION_DECL)
		{
			Function* func = (Function*)c;
			if (strcmp(func->identifier->value, name) == 0)
			{
				return func;
			}
		}
	}

	return NULL;
}

const char* XMA_Expression_ExtractModelName(Expression* expr)
{
	if (expr->Operator() != OP_TYPE_ASSIGN)
		return NULL;

	Group* op1_group = (Group*)expr->Children()->HeadNode()->Owner();
	Group* op2_group = (Group*)expr->Children()->HeadNode()->PrevNode()->Owner();
	if (op1_group->Type() != S_TYPE_GROUP || op2_group->Type() != S_TYPE_GROUP)
		return NULL;

	Literal* op2_val = (Literal*)op2_group->Children()->HeadNode()->Owner();
	if (op2_val->Type() != S_TYPE_LITERAL_STRING)
		return NULL;

	//
	// These checks aren't really necessary as all xmodelaliases scripts follow the same pattern
	//
	Member* array_expr = (Member*)op1_group->Children()->HeadNode()->Owner();
	if (array_expr->Type() != S_TYPE_MEMBER_ARRAY_ELEMENT)
		return NULL;

	Identifier* identifier = (Identifier*)array_expr->Children()->HeadNode()->Owner();
	if (identifier->Type() != S_TYPE_IDENTIFIER)
		return NULL;

	// the model names are always assigned to an array element of a[]
	if (strcmp(identifier->value, "a") != 0)
		return NULL;

	return op2_val->value;
}

void AST_GenCSV_XModelAlias(Symbol* AST, const char* rawfile, FILE* csv)
{
	Function* main = AST_FindFunction(AST, "main");
	if (!main)
	{
		Con_Warning("script does not contain main()\n");
		return;
	}

	Symbol* statements = main->Children()->HeadNode()->PrevNode()->Owner();
	if (statements->Size() < 1)
	{
		Con_Warning("main() function doesn't contain any statments\n");
		return;
	}

	std::set<std::string> models;
	Symbol_EnumChildren(statements, [&models](Symbol* statement) -> bool
	{
		//
		// Skip anything that isnt an expression
		//
		if (statement->Type() != S_TYPE_EXPRESSION)
			return true;

		const char* model = XMA_Expression_ExtractModelName((Expression*)statement);
		
		//
		// Skip any statements that don't contain valid data for this
		//
		if (!model)
			return true;

		std::string modelname = model + 1;
		modelname[modelname.size() - 1] = '\0';

		models.insert(modelname);
		return true;
	});

	fprintf(csv, "rawfile,xmodelalias/%s\n", rawfile);
	for (const std::string& model : models)
	{
		fprintf(csv, "xmodel,%s\n", model.c_str());
	}
}

void AST_GenCSV_AIType(Symbol* AST, const char* rawfile, FILE* csv)
{
	Function* main = AST_FindFunction(AST, "main");
	if (!main)
	{
		Con_Warning("script does not contain main()\n");
		return;
	}

	Symbol* main_statements = main->Children()->HeadNode()->PrevNode()->Owner();
	if (main_statements->Size() < 1)
	{
		Con_Warning("main() function doesn't contain any statments\n");
		return;
	}

	Function* precache = AST_FindFunction(AST, "precache");
	if (!precache)
	{
		Con_Warning("script does not contain precache()\n");
		return;
	}

	Symbol* precache_statements = precache->Children()->HeadNode()->PrevNode()->Owner();
	if (precache_statements->Size() < 1)
	{
		Con_Warning("precache() function doesn't contain any statments\n");
		return;
	}

	std::set<std::string> weapons;
	std::set<std::string> includes;
	std::set<std::string> characters;

	Symbol_EnumChildren(main_statements, [&includes](Symbol* statement) -> bool
	{
		Expression* expr = (Expression*)statement;
		if (expr->Type() != S_TYPE_EXPRESSION)
			return true;

		if (expr->Operator() != OP_TYPE_ASSIGN)
			return true;

		Group* op1_group = (Group*)expr->Children()->HeadNode()->Owner();
		Group* op2_group = (Group*)expr->Children()->HeadNode()->PrevNode()->Owner();
		if (op1_group->Type() != S_TYPE_GROUP || op2_group->Type() != S_TYPE_GROUP)
			return true;

		//
		// Skip if the assigned value isn't a string
		//
		Literal* op2_val = (Literal*)op2_group->Children()->HeadNode()->Owner();
		if (op2_val->Type() != S_TYPE_LITERAL_STRING)
			return true;
		
		//
		// Skip if this expression isn't assigning to a struct property
		//
		Member* prop_expr = (Member*)op1_group->Children()->HeadNode()->Owner();
		if (prop_expr->Type() != S_TYPE_MEMBER_OBJECT_PROPERTY)
			return true;

		Identifier* prop_expr_args = (Identifier*)prop_expr->Children();

		if (prop_expr_args->Type() != S_TYPE_IDENTIFIER || prop_expr_args->NextElem()->Type() != S_TYPE_IDENTIFIER)
			return true;

		//
		// Skip if the we're not assigning to self
		//
		if (strcmp(prop_expr_args->value, "self") != 0)
			return true;

		//
		// Skip if this expression isn't assigning to the csvInclude property
		//
		if (strcmp(static_cast<Identifier*>(prop_expr_args->NextElem())->value, "csvInclude") != 0)
			return true;

		std::string str = op2_val->value + 1;
		
		//
		// Skip if the string is empty (The only thing that would be in the string here would be a quote char)
		//
		if (str.size() <= 1)
			return true;

		//
		// Skip if its not a csv file
		//
		auto offset = str.find_last_of(".");
		if (offset != std::string::npos)
			str[offset] = '\0'; // Strip the extension and trailing quote
		else
			str[str.size() - 1] = '\0'; // Otherwise strip the just trailing quote

		includes.insert(str);
		return true;
	});

	Symbol_EnumChildren(precache_statements, [&characters, &weapons](Symbol* statement) -> bool
	{
		//
		// Function calls reside inside groups
		//
		if (statement->Type() != S_TYPE_GROUP)
			return true;

		if (statement->Size() < 1)
			return true;

		Function* call = (Function*)statement->Children()->HeadNode()->Owner();

		//
		// Skip anything that isnt an expression
		//
		if (call->Type() != S_TYPE_FUNCTION_CALL)
			return true;

		//
		// Cheap way to get the weapons
		//
		if (call->Children()->HeadNode()->Owner()->Type() == S_TYPE_IDENTIFIER)
		{
			Identifier* identifier = (Identifier*)call->Children()->HeadNode()->Owner();
			Group* arg_group = (Group*)call->Children()->NextElem()->Children();
			
			if (strcmp(identifier->value, "precacheItem") != 0)
				return true;

			if (arg_group->Children() == NULL)
				return true;

			Symbol* args = arg_group->Children();

			int argc = args->Size() + 1; // The size of a linked list never includes the head node
			if (argc != 1)
			{
				Con_Warning("Wrong number of args for precacheItem()\n");
				return true;
			}

			if (args->Type() != S_TYPE_LITERAL_STRING)
			{
				Con_Warning("Wrong type of args for precacheItem() %d\n", args->Size());
				return true;
			}

			Literal* string = (Literal*)args;
			std::string weapon = string->value + 1;
			weapon[weapon.size() - 1] = '\0';

			weapons.insert(weapon);
			return true;
		}

		if (call->Children()->HeadNode()->Owner()->Type() == S_TYPE_REFERENCE)
		{
			Reference* ref = (Reference*)call->Children()->HeadNode()->Owner();

			if (strcmp(ref->identifier->value, "precache") != 0)
				return true;

			const char* substr = "character\\";
			if (strstr(ref->file->value, substr) != ref->file->value)
			{
				if (strstr(ref->file->value, "character/") != ref->file->value)
					return true;
			}

			std::string character = ref->file->value + strlen(substr);
			characters.insert(character);
			return true;
		}

		return true;
	});

	fprintf(csv, "rawfile,aitype/%s\n", rawfile);
	for (const std::string& character : characters)
	{
		fprintf(csv, "character,%s\n", character.c_str());
	}

	for (const std::string& weapon : weapons)
	{
		fprintf(csv, "weapon,sp/%s\n", weapon.c_str());
	}

	for (const std::string& include : includes)
	{
		fprintf(csv, "include,%s\n", include.c_str());
	}
}

void AST_GenCSV_Character(Symbol* AST, const char* rawfile, FILE* csv)
{
	std::string csc_path = AppInfo_RawDir();
	csc_path += "\\character\\clientscripts\\";
	csc_path += rawfile;
	
	if (csc_path[csc_path.size() - 3] == 'g')
		csc_path[csc_path.size() - 3] = 'c';
	else if (csc_path[csc_path.size() - 3] == 'G')
		csc_path[csc_path.size() - 3] = 'C';
	
	if (!FS_FileExists(csc_path.c_str()))
		csc_path.clear();

	Function* precache = AST_FindFunction(AST, "precache");
	if (!precache)
	{
		Con_Warning("script does not contain precache()\n");
		return;
	}

	Symbol* precache_statements = precache->Children()->HeadNode()->PrevNode()->Owner();
	if (precache_statements->Size() < 1)
	{
		Con_Warning("precache() function doesn't contain any statments\n");
		return;
	}

	std::set<std::string> models;
	std::set<std::string> xmodelaliases;
	Symbol_EnumChildren(precache_statements, [&models, &xmodelaliases](Symbol* statement) -> bool
	{
		//
		// Function calls reside inside groups
		//
		if (statement->Type() != S_TYPE_GROUP)
			return true;

		if (!statement->Children() || statement->Children()->Size() + 1 < 1)
			return true;

		Function* call = (Function*)statement->Children()->HeadNode()->Owner();

		//
		// Skip anything that isnt an expression
		//
		if (call->Type() != S_TYPE_FUNCTION_CALL)
			return true;

		//
		// Cheap way to get the models
		//
		if (call->Children()->HeadNode()->Owner()->Type() == S_TYPE_IDENTIFIER)
		{
			Identifier* identifier = (Identifier*)call->Children()->HeadNode()->Owner();
			Group* arg_group = (Group*)call->Children()->NextElem()->Children();

			if (strcmp(identifier->value, "precacheModel") != 0)
				return true;

			if (arg_group->Children() == NULL)
				return true;

			Symbol* args = arg_group->Children();

			int argc = args->Size() + 1; // The size of a linked list never includes the head node
			if (argc != 1)
			{
				Con_Warning("Wrong number of args for precacheItem()\n");
				return true;
			}

			if (args->Type() != S_TYPE_LITERAL_STRING)
			{
				Con_Warning("Wrong type of args for precacheItem()\n");
				return true;
			}

			Literal* string = (Literal*)args;
			std::string model = string->value + 1;
			model[model.size() - 1] = '\0';

			models.insert(model);
			return true;
		}

		if (call->Children()->HeadNode()->Owner()->Type() == S_TYPE_REFERENCE)
		{
			Reference* ref = (Reference*)call->Children()->HeadNode()->Owner();
		
			if (strcmp(ref->file->value, "codescripts\\character") != 0)
				return true;

			if (strcmp(ref->identifier->value, "precacheModelArray") != 0)
				return true;

			Group* arg_group = (Group*)call->Children()->NextElem()->Children();

			if (arg_group->Children() == NULL)
				return true;

			Symbol* args = arg_group->Children();

			int argc = args->Size() + 1; // The size of a linked list never includes the head node
			if (argc != 1)
			{
				Con_Warning("Wrong number of args for precacheModelArray()\n");
				return true;
			}

			//
			// The secondary call to the xmodelalias::main()
			//
			call = (Function*)args;
			if (call->Type() != S_TYPE_FUNCTION_CALL)
			{
				Con_Warning("Wrong type of args for precacheModelArray()\n");
				return true;
			}

			ref = (Reference*)call->Children()->HeadNode()->Owner();
			if (ref->Type() != S_TYPE_REFERENCE)
				return true;

			if (strcmp(ref->identifier->value, "main") != 0)
				return true;

			std::string alias = ref->file->value;
			if (alias.find("xmodelalias\\") == 0)
				alias = alias.c_str() + strlen("xmodelalias\\");

			xmodelaliases.insert(alias);
			return true;
		}

		return true;
	});

	fprintf(csv, "rawfile,character/%s\n", rawfile);
	for (const std::string& model : models)
	{
		fprintf(csv, "xmodel,%s\n", model.c_str());
	}

	for (const std::string& xmodelalias : xmodelaliases)
	{
		fprintf(csv, "xmodelalias,%s\n", xmodelalias.c_str());
	}

	if (csc_path.length() > 0)
	{
		fprintf(csv, "rawfile,character/clientscripts/%s", FS_GetFilenameSubString(csc_path.c_str()));
	}
}

int CSVGen_Callback(const char* filePath, const char* fileName, void(*ast_callback)(Symbol* AST, const char* rawfile, FILE* csv))
{
	std::string csv_path = filePath;
	csv_path[csv_path.length() - 3] = 'c';
	csv_path[csv_path.length() - 2] = 's';
	csv_path[csv_path.length() - 1] = 'v';

	if (!fs_overwrite.ValueBool() && FS_FileExists(csv_path.c_str()))
	{
		Con_Print("File '%s' already exists - skipping...\n", FS_GetFilenameSubString(csv_path.c_str()));
		return 1;
	}

	FILE* h;
	fopen_s(&h, filePath, "rb");

	if (!h)
	{
		Con_Warning("File '%s' could not be opened - skipping...\n", fileName);
		return -1;
	}

	const char* typeString = "<unknown>";
	if (ast_callback == AST_GenCSV_AIType)
		typeString = "AIType";
	else if (ast_callback == AST_GenCSV_Character)
		typeString = "Character";
	else if (ast_callback == AST_GenCSV_XModelAlias)
		typeString = "XModelAlias";

	Con_Print("%s (%s)\n", fileName, typeString);

	int fileSize = FS_FileSize(filePath);
	char* buf = new char[fileSize];

	fread(buf, 1, fileSize, h);
	fclose(h);

	yyscan_t scanner = NULL;
	yylex_init(&scanner);

	yy_scan_bytes(buf, fileSize, scanner);
	delete[] buf;

	Symbol* AST = NULL;
	int err = yyparse(&AST, scanner);
	yylex_destroy(scanner);

	FILE* csv = fopen(csv_path.c_str(), "w");
	if (csv)
	{
		ast_callback(AST, FS_GetFilenameSubString(filePath), csv);
		fclose(csv);
	}
	else
	{
		Con_Error("Could not open '%s' for writing...\n", FS_GetFilenameSubString(csv_path.c_str()));
	}
	
	delete AST;

	return 0;
}

int CSVGen_AIType_Callback(const char* filePath, const char* fileName)
{
	return CSVGen_Callback(filePath, fileName, AST_GenCSV_AIType);
}

int CSVGen_Character_Callback(const char* filePath, const char* fileName)
{
	return CSVGen_Callback(filePath, fileName, AST_GenCSV_Character);
}

int CSVGen_XModelAlias_Callback(const char* filePath, const char* fileName)
{
	return CSVGen_Callback(filePath, fileName, AST_GenCSV_XModelAlias);
}

int Cmd_CSVGen_f(int argc, char** argv)
{
	// Automatically iterate over all files for the enabled types
	bool autoMode = false;

	// True if the user manually defined specific types to handle
	bool explicitType = (csvgen_aitypes.ValueBool() | csvgen_characters.ValueBool() | csvgen_xmodelaliases.ValueBool());

	//
	// If no explicit types were given - or an asterisk was the only arg - enable automode for all types
	// If the user doesnt explicitly provide files to be parsed or
	// the user provides only an asterisk as the file
	// we automatically assume that we need to enter autoMode to automatically parse all files for the given types
	//
	if (argc == 1 || (argc == 2 && strcmp(argv[1], "*") == 0))
	{
		autoMode = true;

		// If no explicit types were given - we enable all types
		if (!explicitType)
		{
			csvgen_aitypes.Enable();
			csvgen_characters.Enable();
			csvgen_xmodelaliases.Enable();
		}
	}

	//
	// Automatic file mode logic
	//
	if (autoMode)
	{
		if (argc < 1 || argc > 2)
		{
			char* _argv[] = { NULL, "csvgen" };
			Cmd_Help_f(ARRAYSIZE(_argv), _argv);
			return -1;
		}

		char dir_path[MAX_PATH];
		if (csvgen_aitypes.ValueBool())
		{
			sprintf_s(dir_path, "%s\\aitype", AppInfo_RawDir());
			FS_FileIterator(dir_path, "*.gsc", CSVGen_AIType_Callback);
		}

		if (csvgen_characters.ValueBool())
		{
			sprintf_s(dir_path, "%s\\character", AppInfo_RawDir());
			FS_FileIterator(dir_path, "*.gsc", CSVGen_Character_Callback);
		}
		
		if (csvgen_xmodelaliases.ValueBool())
		{
			sprintf_s(dir_path, "%s\\xmodelalias", AppInfo_RawDir());
			FS_FileIterator(dir_path, "*.gsc", CSVGen_XModelAlias_Callback);
		}

		return 0;
	}

	//
	// Explicit file mode logic
	//
	// We only want to run csv gen for a specific files
	//	If only 1 type is given via args, use that type, otherwise print a warning and attempt to resolve the type automatically
	//
	int typeFlags = 0;
	typeFlags |= csvgen_aitypes.ValueBool() ? 1 << 0 : 0;
	typeFlags |= csvgen_characters.ValueBool() ? 1 << 1 : 0;
	typeFlags |= csvgen_xmodelaliases.ValueBool() ? 1 << 2 : 0;

	for (int i = 0, bitCount = 0; i < sizeof(typeFlags) * 8; i++)
	{
		bitCount += (typeFlags >> i) & 1;
		if (bitCount > 1)
		{
			Con_Warning("Warning: Multiple type arguments given. Falling back to automatic type resolution mode.\n");
			explicitType = false;
			break;
		}
	}

	for (int i = 1; i < argc; i++)
	{
		const char* filepath = argv[i];
		const char* filename = FS_GetFilenameSubString(argv[i]);

		//
		// Automatic Type Resolution
		//
		if (!explicitType)
		{
			if (!FS_FileExists(filepath))
				Con_Warning("File '%s' does not exist... skipping\n", filename);
			else if (csvgen_aitypes.ValueBool() && strstr(argv[i], "aitype") != 0)
				CSVGen_AIType_Callback(filepath, filename);
			else if (csvgen_characters.ValueBool() && strstr(argv[i], "character") != 0)
				CSVGen_Character_Callback(filepath, filename);
			else if (csvgen_xmodelaliases.ValueBool() && strstr(argv[i], "xmodelalias") != 0)
				CSVGen_XModelAlias_Callback(filepath, filename);
			else 
				Con_Warning("Unable to resolve type for file '%s'... skipping\n", filename);
				
			continue;
		}

		//
		// Explicit type logic
		//
		switch (typeFlags)
		{
		case 1 << 0:
			CSVGen_AIType_Callback(filepath, filename);
			break;
		case 1 << 1:
			CSVGen_Character_Callback(filepath, filename);
			break;
		case 1 << 2:
			CSVGen_XModelAlias_Callback(filepath, filename);
			break;
		default:
			Con_Warning("Warning: Invalid type flags...");
		}
	}

	//
	// This should never be reached in automatic file mode
	// Thus - there should always be at least 1 file provided
	//
	if (argc < 2)
	{
		char* _argv[] = { NULL, "csvgen" };
		Cmd_Help_f(ARRAYSIZE(_argv), _argv);
		return -1;
	}

	return 0;
}
