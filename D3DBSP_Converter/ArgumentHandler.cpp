#include "ArgumentHandler.h"
#include "ConsoleLog.h"

int strcmp_case_insensitive(const char* str1, const char* str2)
{
	char* tmp_str1 = new char[strlen(str1)+1];
	memcpy(tmp_str1,str1,strlen(str1)+1);
	for(DWORD i = 0; i < strlen(tmp_str1); i++)
	{
		tmp_str1[i] = toupper(tmp_str1[i]);
	}

	char* tmp_str2 = new char[strlen(str2)+1];
	memcpy(tmp_str2,str2,strlen(str2)+1);
	for(DWORD i = 0; i < strlen(tmp_str2); i++)
	{
		tmp_str2[i] = toupper(tmp_str2[i]);
	}

	int result = strcmp(tmp_str1,tmp_str2);

	delete[] tmp_str1;
	delete[] tmp_str2;

	return result;
}

int HasArg_Logfile(int argc, char** argv)
{
	if(argc > 2)
	{
		for(int i = 2; i < argc; i++)
		{
			if(strcmp_case_insensitive(argv[i],"-l") == 0)
				return 1;
			else if(strcmp_case_insensitive(argv[i],"-log") == 0)
				return 1;
			else if(strcmp_case_insensitive(argv[i],"-logfile") == 0)
				return 1;
		}
	}
	return 0;
}

int HasArg_Restore(int argc, char** argv)
{
	if(argc > 2)
	{
		for(int i = 2; i < argc; i++)
		{
			if(strcmp_case_insensitive(argv[i],"-r") == 0)
				return 1;
			else if(strcmp_case_insensitive(argv[i],"-restore") == 0)
				return 1;
		}
	}
	return 0;
}

int HasArg_Backup(int argc, char** argv)
{
	if(argc > 2)
	{
		for(int i = 2; i < argc; i++)
		{
			if(strcmp_case_insensitive(argv[i],"-b") == 0)
				return 1;
			else if(strcmp_case_insensitive(argv[i],"-backup") == 0)
				return 1;
			else if(strcmp_case_insensitive(argv[i],"-b+") == 0)
				return 1;
			else if(strcmp_case_insensitive(argv[i],"-backup+") == 0)
				return 1;
		}
	}
	return 0;
}

int HasArg_OverwriteBackup(int argc, char** argv)
{
	if(argc > 2)
	{
		for(int i = 2; i < argc; i++)
		{
			if(strcmp_case_insensitive(argv[i],"-b+") == 0)
				return 1;
			else if(strcmp_case_insensitive(argv[i],"-backup+") == 0)
				return 1;
		}
	}
	return 0;
}