#include "arg.h"
#include <string>

bool ARG_FLAG_AUDIO = false;		// -a
bool ARG_FLAG_CONVERT = false;		// -c
bool ARG_FLAG_EVERY = false;		// -e
bool ARG_FLAG_IMAGE = false;		// -i
bool ARG_FLAG_FF = false;			// -r
bool ARG_FLAG_OVERWRITE = false;	// -o
bool ARG_FLAG_SETUP = false;		// -s
bool ARG_FLAG_VERBOSE = false;		// -v

void Arg_PrintUsage()
{
	printf	(	"Usage: setup [OPTION]...\n" \
				"  -s	Run setup\n" \
				"  -c	Run converter\n" \
				"  -e	Extract All Files from IWDs\n" \
				"  -a	Extract Audio Files from IWDs\n" \
				"  -i	Extract Image Files from IWDs\n" \
				"  -r	Extract Rawfiles from FastFiles\n" \
				"  -o	Force overwriting any existing file when extracting\n" \
				"  -v	Verbose print mode\n" \
			);
}

int Arg_HandleArguments(int argc, char** argv)
{
	if (argc != 2)
	{
		Arg_PrintUsage();
		return -1;
	}

	char* argList = argv[1];
	for (unsigned int i = 0; i < strlen(argList); i++)
	{
		argList[i] = toupper(argList[i]);

		switch (argList[i])
		{
			case 'A':
				ARG_FLAG_AUDIO = true;
				break;
			case 'E':
				ARG_FLAG_EVERY = true;
				break;
			case 'I':
				ARG_FLAG_IMAGE = true;
				break;
			case 'R':
				ARG_FLAG_FF = true;
				break;
			case 'O':
				ARG_FLAG_OVERWRITE = true;
				break;
			case 'S':
				ARG_FLAG_SETUP = true;
				//break; //converter always needs to run post setup
			case 'C':
				ARG_FLAG_CONVERT = true;
				break;
			case 'V':
				ARG_FLAG_VERBOSE = true;
				break;
			default:
				break;
		}
	}

	return 0;
}