#include "gui_view.h"
#include "imgui\imgui.h"

static bool selected = false;
const float height = 16;

void GUIView_Alias_DrawTextBox(const char* label, void* data, float column_width )
{
	ImVec2 size(column_width, height);
	float pos_x = ImGui::GetCursorPosX();
	ImGui::InputText(label, (char*)data, 16, 0); // TODO: Fix this (possibly use multi line and restrict it to 1 line)
	ImGui::SetCursorPosX(pos_x + column_width);
}

void GUIView_Alias_DrawComboBox(const char* label, void* data, float column_width)
{
	ImVec2 size(column_width, height);
	ImGui::Selectable("COMBO", &selected, 0, size);
}

void GUIView_Alias_DrawKnob(const char* label, void* data, float column_width)
{
	ImVec2 size(column_width, height);
	ImGui::Selectable("KNOB", &selected, 0, size);
}

void GUIView_Alias_DrawCheckBox(const char* label, void* data, float column_width)
{
	ImVec2 size(column_width, 0);
	float pos_x = ImGui::GetCursorPosX();
	ImGui::Checkbox(label, (bool*)data);
	ImGui::SetCursorPosX(pos_x + column_width);
}

void GUIView_Alias_DrawNumber(const char* label, void* data, float column_width)
{
	ImVec2 size(column_width, height);
	ImGui::Selectable("NUMBER", &selected, 0, size);
}

void GUIView_Alias_DrawCheckList(const char* label, void* data, float column_width)
{
	ImVec2 size(column_width, height);
	ImGui::Selectable("CHECKLIST", &selected, 0, size);
}

