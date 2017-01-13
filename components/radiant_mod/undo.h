#pragma once

extern void(*Undo_Start_o)(const char *undoName);
extern void(*Undo_End_o)();

void hk_Undo_Start(const char *undoName);
void hk_Undo_End();