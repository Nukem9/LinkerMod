#include "gui_view.h"
#include "imgui\imgui.h"

static bool selected = false;
const float height = 16;

void GUIView_Alias_DrawTextBox(const char* label, void* data, void* metadata, float column_width )
{
	ImVec2 size(column_width, height);
	float pos_x = ImGui::GetCursorPosX();
	csv_metadata_s* meta = (csv_metadata_s*)metadata;
	ImGui::InputText(label, (char*)data, meta->entry->length, 0); // TODO: Fix this (possibly use multi line and restrict it to 1 line)
	ImGui::SetCursorPosX(pos_x + column_width);
}

void GUIView_Alias_DrawComboBox(const char* label, void* data, void* metadata, float column_width)
{
	ImVec2 size(column_width, height);
	csv_metadata_s* meta = (csv_metadata_s*)metadata;

	ImGui::Combo(label, (int*)data, meta->enum_string);
}

void GUIView_Alias_DrawKnob(const char* label, void* data, void* metadata, float column_width)
{
	ImVec2 size(column_width, height);
	csv_metadata_s* meta = (csv_metadata_s*)metadata;

	char buf[256]; 
	sprintf(buf, "%%.3f %s", meta->displayUnit);

	ImGui::SliderFloat(label, (float*)data, meta->_min, meta->_max, buf);
	*(float*)data = fminf(*(float*)data, meta->_max);
	*(float*)data = fmaxf(*(float*)data, meta->_min);
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
	csv_metadata_s* meta = (csv_metadata_s*)metadata;

	int val = 0;
	switch (meta->entry->type)
	{
	case CSV_USHORT:
		val = *(unsigned short*)data;
		ImGui::DragInt(label, &val, 1.0f, (int)meta->_min, (int)meta->_max);
		val = val > meta->_min ? val : (int)meta->_min;
		val = val < meta->_max ? val : (int)meta->_max;
		*(unsigned short*)data = val;
		break;
	default:
		break;
	}
	
}

void GUIView_Alias_DrawCheckList(const char* label, void* data, void* metadata, float column_width)
{
	ImVec2 size(column_width, height);
	ImGui::Selectable("CHECKLIST", &selected, 0, size);
}

