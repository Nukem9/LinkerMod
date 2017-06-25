#include "stdafx.h"

void(*Undo_Start_o)(const char *undoName);
void(*Undo_End_o)();

void hk_Undo_Start(const char *undoName)
{
	//if (!_stricmp(undoName, "create entity"))
	//	Remote_AddEntityCreate(nullptr);

	Undo_Start_o(undoName);
}

void hk_Undo_End()
{
	Undo_End_o();
	Remote_CommitChanges();
}