#pragma once
#include "../csv/csv.h"

class GUIGridView
{
private:
	CSVStaticTable* table;

public:
	GUIGridView(void);
	~GUIGridView(void);
	
	int LoadTable(void); // Load the test table
	int LoadTable(const char* path);

	void Draw(void);
};