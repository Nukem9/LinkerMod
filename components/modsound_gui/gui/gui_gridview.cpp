#include "gui_gridview.h"
#include <Windows.h>
#include "../sys/AppInfo.h"
#include "imgui\imgui.h"

GUIGridView::GUIGridView(void) : table(NULL)
{
}

GUIGridView::~GUIGridView(void)
{
	delete this->table;
}

int GUIGridView::LoadTable(void)
{
	char path[MAX_PATH];
	sprintf_s(path, "%s\\raw\\soundaliases\\zmb_music.csv", AppInfo_AppDir());

	return this->LoadTable(path);
}

int GUIGridView::LoadTable(const char* path)
{
	this->table = new CSVStaticTable(path, CSV_ST_PRUNE_EMPTY);
	return 0;
}

void GUIGridView::Draw(void)
{
	if (this->table == NULL)
	{
		return;
	}

	bool selected = false;
	ImVec2 size(256, 16.0);

	ImGuiListClipper clipper(table->RowCount(), -1);
	while (clipper.Step())
	{
		int start_c = -1;
		int end_c = -1;

		for (int r = clipper.DisplayStart; r < clipper.DisplayEnd; r++) // draw each row
		{
			float offset = 0;
			for (int c = 0; c < table->FieldCount(); c++)
			{
				if (c > 0)
					ImGui::SameLine();

				ImGui::Selectable(table->CellValue(r, c), &selected, 0, size);
			}
		}
	}

	clipper.End();
}
