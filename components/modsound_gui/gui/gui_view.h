#pragma once
#include "../csv/csv.h"
#include "../csv/csv_alias.h"
#include "../csv/csv_metadata.h"

template <typename aT>
struct GUIView_WidgetMapping
{
	const char* widget_name;
	void(*draw_func)(aT* arg, float column_width);
};

void GUIView_Alias_DrawTextBox(const char* label, void* data, float column_width);
void GUIView_Alias_DrawComboBox(const char* label, void* data, float column_width);
void GUIView_Alias_DrawKnob(const char* label, void* data, float column_width);
void GUIView_Alias_DrawCheckBox(const char* label, void* data, float column_width);
void GUIView_Alias_DrawNumber(const char* label, void* data, float column_width);

void GUIView_Alias_DrawCheckList(const char* label, void* data, float column_width);
