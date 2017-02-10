#include "gui_view.h"
#include "imgui\imgui.h"

static bool selected = false;
const float height = 16;

void GUIView_Alias_DrawTextBox(const char* label, void* data, void* metadata, float column_width )
{
	ImVec2 size(column_width, height);
	float pos_x = ImGui::GetCursorPosX();
	ImGui::InputText(label, (char*)data, 16, 0); // TODO: Fix this (possibly use multi line and restrict it to 1 line)
	ImGui::SetCursorPosX(pos_x + column_width);
}

void GUIView_Alias_DrawComboBox(const char* label, void* data, void* metadata, float column_width)
{
	ImVec2 size(column_width, height);
	csv_metadata_s* meta = (csv_metadata_s*)metadata;

	if (!meta->entry->enums)
	{
		ImVec2 size(column_width, height);
		ImGui::Selectable("ERR COMBO", &selected, 0, size);
		return;
	}

	char buf[1024];
	char* p = buf;
	for (const char** c = meta->entry->enums; *c; c++)
	{
		int len = sprintf_s(p, ARRAYSIZE(buf) - (p - buf), "%s", *c);
		p += len + 1;
	}
	*p = '\0';
	//meta->entry->enums
	ImGui::Combo(label, (int*)data, buf);
}

void GUIView_Alias_DrawKnob(const char* label, void* data, void* metadata, float column_width)
{
	ImVec2 size(column_width, height);
	ImGui::Selectable("KNOB", &selected, 0, size);
}

void GUIView_Alias_DrawCheckBox(const char* label, void* data, void* metadata, float column_width)
{
	ImVec2 size(column_width, 0);
	float pos_x = ImGui::GetCursorPosX();
	ImGui::Checkbox(label, (bool*)data);
	ImGui::SetCursorPosX(pos_x + column_width);
}

void GUIView_Alias_DrawNumber(const char* label, void* data, void* metadata, float column_width)
{
	ImVec2 size(column_width, height);
	ImGui::Selectable("NUMBER", &selected, 0, size);
}

void GUIView_Alias_DrawCheckList(const char* label, void* data, void* metadata, float column_width)
{
	ImVec2 size(column_width, height);
	ImGui::Selectable("CHECKLIST", &selected, 0, size);
}

