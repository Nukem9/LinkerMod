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

void AST_ExtractXModelNames(const char* rawfile, Symbol* AST)
{
	AST->PrintInfoRecursive();

	Function* main = AST_FindFunction(AST, "main");
	if (!main)
	{
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
		AST_ExtractXModelNames(FS_GetFilenameSubString(filepath), AST);
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
