#pragma once 
#include <vector>

template<typename T>
class ICSVTable
{
protected:
	std::vector<std::vector<T>> cells;

public:
	int FieldCount(void) const;
	T& FieldName(int field_index) const;

	int RowCount(void) const;
	std::vector<T>& RowValue(int row_index) const;

	T& CellValue(int row_index, int field_index) const;

	bool isEmpty(void) const;

	virtual int ReadFile(const char* path) = 0;
	virtual int WriteFile(const char* path, bool overwrite = false) const = 0;

	virtual void PrintTable(FILE* h = stdout, bool include_debug_info = false) const = 0;
};

enum CSV_ST_LOADFLAGS
{
	CSV_ST_DEFAULT = 0,
	CSV_ST_PRUNE_EMPTY =			1 << 0,
	CSV_ST_HEADERLESS_SINGLEFIELD =	1 << 1, // Automatically add the 'name' as the table doesnt contain any field names
	CSV_ST_PRUNE_COMMENTS =		1 << 2, // Ignore header / comment lines
};

//
// Optimized for loading CSV tables that will not be modified
//
class CSVStaticTable : public ICSVTable<const char*>
{
private:
	// std::vector<std::vector<const char*>> cells;
	char* buf;

public:
	CSVStaticTable(void);
	CSVStaticTable(const char* path, int loadflags = CSV_ST_DEFAULT);
	~CSVStaticTable(void);

	//int FieldCount(void) const;
	//const char* FieldName(int field_index) const;
	const char* FieldName(int field_index) const;

	//int RowCount(void) const;
	//const char* CellValue(int row_index, int field_index) const;
	const char* CellValue(int row_index, int field_index) const;

	//
	// Remove all empty rows from the CSV table (can also be passed additional bits to prune comments too)
	// returns the number of rows removed
	//
	int PruneRows(int bits = CSV_ST_PRUNE_EMPTY);
	void DeleteRow(int row_index);

	//
	// Remove all empty columns from the table
	// returns the number of columns removed
	//
	int PruneColumns(void);

	int ReadFile(const char* path) override;
	int ReadFile(const char* path, int loadflags);
	int WriteFile(const char* path, bool overwrite = false) const override;

	void PrintTable(FILE* h = stdout, bool include_debug_info = false) const override;;
};

template<typename T>
int ICSVTable<T>::FieldCount(void) const
{
	_ASSERT(this->cells.size());
	return this->cells[0].size();
}

template<typename T>
T& ICSVTable<T>::FieldName(int field_index) const
{
	_ASSERT(field_index < this->FieldCount());
	return this->cells[0][field_index];
}

template<typename T>
int ICSVTable<T>::RowCount(void) const
{
	_ASSERT(this->cells.size() > 1);
	return this->cells.size() - 1;
}

template<typename T>
std::vector<T>& ICSVTable<T>::RowValue(int row_index) const
{
	_ASSERT(row_index < this->RowCount());

	return this->cells[row_index + 1];
}

template<typename T>
T& ICSVTable<T>::CellValue(int row_index, int field_index) const
{
	_ASSERT(row_index < this->RowCount());
	_ASSERT(field_index < this->FieldCount());

	return this->cells[row_index + 1][field_index];
}

template<typename T>
bool ICSVTable<T>::isEmpty(void) const
{
	return this->cells.size() == 0;
}
