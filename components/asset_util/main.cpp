#include <iostream>

#include "assettype\xmodelalias.h"
#include "assettype\aitype.h"
#include "gdt.h"

GDT gdt;

int __cdecl fileCallback(const char* filePath, const char* fileName)
{
	printf("%s\n", fileName);

	//XModelAlias* alias = new XModelAlias;
	//alias->ExtractFromGSC(argv[i]);

	AIType* aitype = new AIType;
	aitype->ExtractFromGSC(filePath);

	gdt.assets.push_back(aitype);
	return 0;

}

int main(int argc, const char** argv)
{
	
	FS_FileIterator(argv[1], "*.gsc", fileCallback);

	gdt.WriteFile("test.gdt", GDT_MODE_OVERWRITE);
	return 0;
}