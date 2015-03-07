#pragma once
#include "Lump.h"

enum BSPVERSION_TYPE
{
	BSPVERSION_COD_WAW = 31,
	BSPVERSION_COD_BO = 45
};

class D3DBSP
{
public:
	DWORD diskLumpCount;

	DWORD diskLumpOrderSize;
	LUMP_TYPE* diskLumpOrder;

	DWORD bspVersion;
	Lump lumps[INFO_LUMP_TYPE_COUNT];

	int Load(const char* filepath);
	int Write(const char* filepath);

	int ConvertFrom(D3DBSP*, BSPVERSION_TYPE);

	D3DBSP(void);
	~D3DBSP(void);
};

