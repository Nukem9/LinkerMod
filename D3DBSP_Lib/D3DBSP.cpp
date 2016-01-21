#include "D3DBSP.h"
#include "Convert.h"
#include "ConsoleLog.h"
#include "IBUF.h"

struct LumpIndexEntry
{
	DWORD type;
	DWORD length;
};

unsigned int padding_size(unsigned int i)
{
	return ((i+3) & 0xFFFFFFFC)-i;
}

unsigned int padded(unsigned int i)
{
	return (i+3) & 0xFFFFFFFC;
}

#include <fstream>
#include <sys\stat.h>
int D3DBSP::Load(const char* filepath)
{
	DWORD fileSize;

	struct stat* results = new struct stat;
	if (stat(filepath, results) != 0)
	{
		Con_Printf("\n");
		Con_Error("File not found\n");
		delete results;
		return 1;// ERR_FILE_NOT_FOUND;
	}
	fileSize = results->st_size;
	delete results;

	std::ifstream ifile(filepath, std::ios::in | std::ios::binary);

	DWORD magicValue = 0;
	ifile.read((char*)&magicValue, sizeof(DWORD));
	if(magicValue != 'PSBI')
	{
		ifile.close();
		return 2; //ERR_NOT_A_D3DBSP_FILE
	}

	ifile.read((char*)&this->bspVersion, sizeof(DWORD));
	ifile.read((char*)&this->diskLumpCount, sizeof(DWORD));

	this->diskLumpOrderSize = this->diskLumpCount;
	this->diskLumpOrder = new LUMP_TYPE[this->diskLumpOrderSize];

	DWORD lumpOffset = sizeof(DWORD)*3 + sizeof(LumpIndexEntry)*this->diskLumpCount;
	for(DWORD i = 0; i < this->diskLumpCount; i++)
	{
		LumpIndexEntry indexEntry;
		ifile.read((char*)&indexEntry, sizeof(LumpIndexEntry));
		
		this->diskLumpOrder[i] = (LUMP_TYPE)indexEntry.type;

		this->lumps[indexEntry.type].size = indexEntry.length;
		this->lumps[indexEntry.type].content = new BYTE[indexEntry.length];
		this->lumps[indexEntry.type].isEmpty = false;

		std::streamoff cLoc = ifile.tellg();
		ifile.seekg(lumpOffset);
		
		ifile.read((char*)this->lumps[indexEntry.type].content, indexEntry.length);
		
		ifile.seekg(cLoc);
		lumpOffset += padded(indexEntry.length);
	}

	ifile.close();
	return 0;
}

int D3DBSP::Load(BYTE* pBuf)
{
	BYTE* BSpBuf = pBuf;

	DWORD magicValue = 0;
	memcpy(&magicValue,pBuf,sizeof(DWORD));
	if(magicValue != 'PSBI')
	{
		Con_Error("Buffer does not contain D3DBSP data\n");
		return 2; //ERR_NOT_A_D3DBSP_FILE
	}

	pBuf+=sizeof(DWORD);

	memcpy((char*)&this->bspVersion, pBuf, sizeof(DWORD));
	pBuf+=sizeof(DWORD);
	memcpy((char*)&this->diskLumpCount, pBuf, sizeof(DWORD));
	pBuf+=sizeof(DWORD);

	this->diskLumpOrderSize = this->diskLumpCount;
	this->diskLumpOrder = new LUMP_TYPE[this->diskLumpOrderSize];

	DWORD lumpOffset = sizeof(DWORD)*3 + sizeof(LumpIndexEntry)*this->diskLumpCount;
	for(DWORD i = 0; i < this->diskLumpCount; i++)
	{
		LumpIndexEntry indexEntry;
		
		memcpy((char*)&indexEntry, pBuf, sizeof(LumpIndexEntry));
		pBuf+=sizeof(LumpIndexEntry);

		this->diskLumpOrder[i] = (LUMP_TYPE)indexEntry.type;

		this->lumps[indexEntry.type].size = indexEntry.length;
		this->lumps[indexEntry.type].content = new BYTE[indexEntry.length];
		this->lumps[indexEntry.type].isEmpty = false;

		BYTE* cLoc = pBuf;
		pBuf = BSpBuf + lumpOffset;
		
		memcpy((char*)this->lumps[indexEntry.type].content, pBuf, indexEntry.length);
		
		pBuf = cLoc;
		lumpOffset += padded(indexEntry.length);
	}

	return 0;
}

int D3DBSP::Write(const char* filepath)
{
	std::ofstream ofile(filepath, std::ios::out | std::ios::binary);
	
	if(!ofile.is_open())
	{
		return 1; //ERR_COULD_NOT_OPEN_FILE
	}

	ofile.write("IBSP", 4);
	ofile.write((char*)&this->bspVersion, sizeof(DWORD));
	ofile.write((char*)&this->diskLumpCount, sizeof(DWORD));

	for(DWORD i = 0; i < this->diskLumpOrderSize; i++)
	{
		Lump* pLump = &this->lumps[this->diskLumpOrder[i]];

		if(!pLump->isEmpty)
		{
			LumpIndexEntry indexEntry;
			indexEntry.length = pLump->size;
			indexEntry.type = this->diskLumpOrder[i];
			
			ofile.write((char*)&indexEntry, sizeof(LumpIndexEntry));
		}
	}

	for(DWORD i = 0; i < this->diskLumpOrderSize; i++)
	{
		Lump* pLump = &this->lumps[this->diskLumpOrder[i]];

		if(!pLump->isEmpty)
		{
#if _DEBUG
			Log_Printf("Writing Lump [0x%X] %s\n", this->diskLumpOrder[i], LUMP_NAMES[this->diskLumpOrder[i]]);
			Log_Printf("	Start: 0x%X (Size: 0x%X)\n", (DWORD)ofile.tellp(), pLump->size);
#endif
			ofile.write((char*)pLump->content, pLump->size);
			if(padding_size(pLump->size))
			{
				DWORD pad = '\0\0\0\0';
				ofile.write((char*)&pad, padding_size(pLump->size));
			}
#if _DEBUG
			Log_Printf("	End: 0x%X\n", (DWORD)ofile.tellp());
#endif
		}
	}

	ofile.close();
	return 0;
}

int D3DBSP::Write(BYTE* pBuf)
{
	IBUF iBuf(pBuf);

	iBuf.write("IBSP",4);
	iBuf.write((char*)&this->bspVersion, sizeof(DWORD));
	iBuf.write((char*)&this->diskLumpCount, sizeof(DWORD));

	for(DWORD i = 0; i < this->diskLumpOrderSize; i++)
	{
		Lump* pLump = &this->lumps[this->diskLumpOrder[i]];

		if(!pLump->isEmpty)
		{
			LumpIndexEntry indexEntry;
			indexEntry.length = pLump->size;
			indexEntry.type = this->diskLumpOrder[i];
			iBuf.write((char*)&indexEntry, sizeof(LumpIndexEntry));
		}
	}

	for(DWORD i = 0; i < this->diskLumpOrderSize; i++)
	{
		Lump* pLump = &this->lumps[this->diskLumpOrder[i]];

		if(!pLump->isEmpty)
		{
			iBuf.write((char*)pLump->content, pLump->size);
			if(padding_size(pLump->size))
			{
				DWORD pad = '\0\0\0\0';
				iBuf.write((char*)&pad, padding_size(pLump->size));
			}
		}
	}

	return 0;
}

unsigned int D3DBSP::PotentialFileSize()
{
	unsigned int size = 0;
	size += sizeof(DWORD)*3;
	size += this->diskLumpOrderSize * sizeof(LumpIndexEntry);

	for(DWORD i = 0; i < this->diskLumpOrderSize; i++)
	{
		Lump* pLump = &this->lumps[this->diskLumpOrder[i]];

		if(!pLump->isEmpty)
		{
			size += pLump->size;
			if(padding_size(pLump->size))
			{
				size += padding_size(pLump->size);
			}
		}
	}

	return size;
}

int D3DBSP::ConvertFrom(D3DBSP* inputBSP, BSPVERSION_TYPE targetVersion)
{
	if(inputBSP->bspVersion == (DWORD)targetVersion)
	{
		return ConvertBSP_Copy(inputBSP, this);
	}

	this->bspVersion = targetVersion;

	switch(inputBSP->bspVersion)
	{
	case BSPVERSION_COD_WAW:
		return ConvertBSP_WAWtoBO(inputBSP, this);
	case BSPVERSION_COD_BO:
		return ConvertBSP_BOtoWAW(inputBSP, this);
	default:
		return 1;
	}
}

int D3DBSP::Convert(BSPVERSION_TYPE targetVersion)
{
	D3DBSP oBSP;
	oBSP.ConvertFrom(this, targetVersion);
	return this->ConvertFrom(&oBSP, targetVersion);
}

Lump* D3DBSP::AddLump(LUMP_TYPE type, Lump& lump)
{
	LUMP_TYPE* newDiskLumpOrder = new LUMP_TYPE[diskLumpCount + 1];
	memcpy(newDiskLumpOrder, this->diskLumpOrder, sizeof(LUMP_TYPE) * diskLumpCount);
	delete[] this->diskLumpOrder;
	
	this->diskLumpOrder = newDiskLumpOrder;
	diskLumpOrder[this->diskLumpCount] = type;
	this->lumps[type] = lump;

	this->diskLumpCount++;
	this->diskLumpOrderSize = diskLumpCount;
	return &this->lumps[type];
}

D3DBSP::D3DBSP(void)
{
	this->diskLumpOrderSize = 0;
	this->diskLumpOrder = nullptr;
}

D3DBSP::~D3DBSP(void)
{
	this->diskLumpOrderSize = 0;
	delete[] this->diskLumpOrder;
}
