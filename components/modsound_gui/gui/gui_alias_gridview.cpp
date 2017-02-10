#include <Windows.h>
#include "../sys/AppInfo.h"
#include "../common/io.h"
#include "gui_view.h"
#include "gui_alias_gridview.h"
#include "../csv/csv_alias.h"

#include "imgui\imgui.h"

//
// This must line up with CSV_WIDGET_TYPE
//
//static GUIView_WidgetMapping<csv_snd_alias_t> gui_wm_alias[CSV_WIDGET_COUNT] =
//{
//	{ "text", GUIView_Alias_DrawTextBox },
//	{ "combo", GUIView_Alias_DrawComboBox },
//	{ "knob", GUIView_Alias_DrawKnob },
//	{ "number", GUIView_Alias_DrawNumber },
//	{ "checkbox", GUIView_Alias_DrawCheckBox },
//	{ "checklist", GUIView_Alias_DrawCheckList },
//};

typedef void(*gui_alias_widgetdrawfunc_f)(const char* label, void* data, void* metadata, float column_width);
gui_alias_widgetdrawfunc_f gui_alias_widgetfuncs[CSV_WIDGET_COUNT] =
{
	GUIView_Alias_DrawTextBox,
	GUIView_Alias_DrawComboBox,
	GUIView_Alias_DrawKnob,
	GUIView_Alias_DrawNumber,
	GUIView_Alias_DrawCheckBox,
	GUIView_Alias_DrawCheckList,
};

GUIAliasGridView::GUIAliasGridView(void)
{
}

GUIAliasGridView::~GUIAliasGridView(void)
{
}

int GUIAliasGridView::LoadTable(void)
{
	char path[MAX_PATH];
	sprintf_s(path, "%s\\raw\\soundaliases\\template.csv", AppInfo_AppDir());
	return this->LoadTable(path);
}



int GUIAliasGridView::LoadTable(const char* path)
{
	const CSVStaticTable table(path, CSV_ST_PRUNE_EMPTY);
	CSV_LoadSoundAlias(&table, &this->aliases);

	this->field_defs.resize(table.FieldCount());

	for (int i = 0; i < table.FieldCount(); i++)
	{
		const char* field = table.FieldName(i);

		csv_metadata_s* metadata = g_metadata->ResolveMetadataForField(field);
		if (!metadata)
		{
			Con_Error("ERROR: Unable to resolve metadata for field '%s'\n", field);
			return 1;
		}

		csv_entry_t* entry = metadata->entry;
		if (!entry)
		{
			Con_Error("ERROR: Metadata for field '%s' is missing entry\n", field);
			return 2;
		}

		field_defs[i].draw_func = gui_alias_widgetfuncs[metadata->widget];
		field_defs[i].data_offset = entry->offset;
		field_defs[i].metadata = metadata;

		//
		//if (csv_metadata_s* metadata = g_metadata->ResolveMetadataForField(field))
		//{
		//	field_defs[i].draw_func = gui_alias_widgetfuncs[metadata->widget];
		//	continue;
		//}
		//else
		//{
		//	return 2;
		//}
		//
		//switch (entry->type)
		//{
		//case CSV_DBSPL:
		//case CSV_CENTS:
		//case CSV_NORM_BYTE:
		//	field_defs[i].draw_func = GUIView_Alias_DrawKnob;
		//	continue;
		//case CSV_FLAG:
		//	field_defs[i].draw_func = GUIView_Alias_DrawCheckBox;
		//	continue;
		//case CSV_ENUM_BYTE:
		//	_ASSERT(entry->enums);
		//	field_defs[i].draw_func = GUIView_Alias_DrawComboBox;
		//	continue;
		//default:
		//	Con_Error("ERROR: Unsupported entry type while initializing widgets for field '%s' (%d)\n", field, entry->type);
		//	return 1;
		//}
	}

	return 0;
}

void GUIAliasGridView::Draw(void)
{
	char id[8];

	ImGuiListClipper clipper(aliases.size(), -1);
	while (clipper.Step())
	{
		unsigned int field_count = this->field_defs.size();
		for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
		{
			for (unsigned int f = 0; f < field_count; f++)
			{
				sprintf_s(id, "##%d", field_defs.size() * i + f);

				BYTE* data = (BYTE*)&aliases[i];
				field_defs[f].draw_func(id, data + field_defs[f].data_offset, field_defs[f].metadata, 32.0f);
	
				if (f + 1 < field_count)
				{
					ImGui::SameLine();
				}
			}
		}
	}

	clipper.End();

	//if (this->table == NULL)
	//{
	//	return;
	//}
	//
	//bool selected = false;
	//ImVec2 size(256, 16.0);
	//
	//ImGuiListClipper clipper(table->RowCount(), -1);
	//while (clipper.Step())
	//{
	//	int start_c = -1;
	//	int end_c = -1;
	//
	//	for (int r = clipper.DisplayStart; r < clipper.DisplayEnd; r++) // draw each row
	//	{
	//		float offset = 0;
	//		for (int c = 0; c < table->FieldCount(); c++)
	//		{
	//			if (c > 0)
	//				ImGui::SameLine();
	//
	//			ImGui::Selectable(table->CellValue(r, c), &selected, 0, size);
	//		}
	//	}
	//}
	//
	//clipper.End();
}
