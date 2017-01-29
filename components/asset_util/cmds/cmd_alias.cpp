#include "../sys/AppInfo.h"
#include "../common/io.h"
#include "../common/fs.h"

#include "../cvar.h"
#include "cmd_common.h"

#include "../gsc_parser/gsc_parser.h"
#pragma comment(lib, "gsc_parser.lib")

#include <vector>
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

void AST_GenCSV_XModelAlias(const char* rawfile, Symbol* AST)
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

	std::vector<std::string> models;
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

		models.push_back(modelname);
		return true;
	});

	Con_Print("rawfile,xmodelalias/%s\r\n", rawfile);
	for (std::string& model : models)
	{
		Con_Print("xmodel,%s\r\n", model.c_str());
	}
}

void AST_GenCSV_AIType(const char* rawfile, Symbol* AST)
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

	std::vector<std::string> weapons;
	std::vector<std::string> includes;
	std::vector<std::string> characters;

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

		includes.push_back(str);
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

			weapons.push_back(weapon);
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
			characters.push_back(character);
			return true;
		}

		return true;
	});

	Con_Print("rawfile,aitype/%s\r\n", rawfile);
	for (std::string& character : characters)
	{
		Con_Print("character,%s\r\n", character.c_str());
	}

	for (std::string& weapon : weapons)
	{
		Con_Print("weapon,sp/%s\r\n", weapon.c_str());
	}

	for (std::string& include : includes)
	{
		Con_Print("include,%s\r\n", include.c_str());
	}
}

int Cmd_Alias_f(int argc, char** argv)
{
	for (int i = 1; i < argc; i++)
	{
		const char* filepath = argv[i];
		if (!FS_FileExists(filepath))
		{
			Con_Warning("File '%s' doesn't exist - skipping...\n", FS_GetFilenameSubString(filepath));
			continue;
		}

		FILE* h;
		fopen_s(&h, filepath, "rb");

		if (!h)
		{
			Con_Warning("File '%s' could not be opened - skipping...\n", FS_GetFilenameSubString(filepath));
			continue;
		}

		int fileSize = FS_FileSize(filepath);
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

		//AST->PrintInfoRecursive();
		//AST_GenCSV_XModelAlias(FS_GetFilenameSubString(filepath), AST);
		AST_GenCSV_AIType(FS_GetFilenameSubString(filepath), AST);
		delete AST;
	}

	if (argc < 2)
	{
		char* _argv[] = { NULL, "alias" };
		Cmd_Help_f(ARRAYSIZE(_argv), _argv);
		return -1;
	}
	return 0;
}
