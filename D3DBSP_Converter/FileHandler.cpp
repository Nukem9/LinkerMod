#include "FileHandler.h"
#include "ConsoleLog.h"

int File_Backup(const char* originalFilePath, BOOL overwriteOldBackup)
{
	char backupFilepath[FILENAME_MAX];

	if(sprintf_s(backupFilepath,"%s.cod5.bak",originalFilePath) == -1)
	{
		Con_Error("ERROR: Buffer Error\n");
		return 1;
	}

	if(!CopyFileA(originalFilePath,backupFilepath,overwriteOldBackup) && !overwriteOldBackup)
	{
		Con_Error("ERROR: Could Not Create Backup File\n");
		return 2;
	}
	
	return 0;
}

int File_RestoreBackup(const char* originalFilePath)
{
	char backupFilepath[FILENAME_MAX];

	if(sprintf_s(backupFilepath,"%s.cod5.bak",originalFilePath) == -1)
	{
		Con_Error("ERROR: Buffer Error\n");
		return 1;
	}

	if(!CopyFileA(backupFilepath,originalFilePath,false))
	{
		Con_Error("ERROR: Could Not Restore File\n");
		return 2;
	}

	return 0;
}