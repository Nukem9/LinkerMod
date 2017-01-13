#include "stdafx.h"

void(*MoveSelection_o)(int a1, int a2, int a3, int a4);

void hk_MoveSelection(int a1, int a2, int a3, int a4)
{
	// Set the list of the originally selected ents
	for (brush_s *b = selected_brushes->next; b != selected_brushes; b = b->next)
	{
		//entity_s *ent = (entity_s *)*(DWORD *)((DWORD)selected_brushes->next->owner + 8);
		entity_s *ent = (entity_s *)*(DWORD *)((DWORD)b->owner + 8);

		if (ent)
			Remote_AddEntityUpdate(ent);
	}

	MoveSelection_o(a1, a2, a3, a4);

	Remote_CommitChanges();
}