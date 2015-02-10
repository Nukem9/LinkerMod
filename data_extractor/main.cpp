//TODO: Rewrite / Clean Code
//Use any arg to overwrite currently existant exported files

#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>

bool overwriteFiles = false;

HANDLE hProcess = nullptr;

struct sPoolPtr //sAssetPtr
{
	int type;
	DWORD ptr;
	BYTE zoneID;
	BYTE O1_2;
	WORD OO;
	int zeros;
};



BYTE* db_hashTable = (BYTE*)0x00CD81F8;
sPoolPtr* AssetList = (sPoolPtr*)0x00DE85D8;
char* zoneNameArray = (char*)0x010C6608;

#include "material.h"

#include <sys/stat.h>
bool FileExists(const char* filename) 
{
	struct stat info;
	int ret = -1;
 
	ret = stat(filename, &info);
	if(ret == 0) 
	{
		return true;
	} 
	else 
	{
		return false;
    }
}

enum XAssetType
{
  ASSET_TYPE_XMODELPIECES = 0x0,
  ASSET_TYPE_PHYSPRESET = 0x1,
  ASSET_TYPE_PHYSCONSTRAINTS = 0x2,
  ASSET_TYPE_DESTRUCTIBLEDEF = 0x3,
  ASSET_TYPE_XANIMPARTS = 0x4,
  ASSET_TYPE_XMODEL = 0x5,
  ASSET_TYPE_MATERIAL = 0x6,
  ASSET_TYPE_TECHNIQUE_SET = 0x7,
  ASSET_TYPE_IMAGE = 0x8,
  ASSET_TYPE_SOUND = 0x9,
  ASSET_TYPE_SOUND_PATCH = 0xA,
  ASSET_TYPE_CLIPMAP = 0xB,
  ASSET_TYPE_CLIPMAP_PVS = 0xC,
  ASSET_TYPE_COMWORLD = 0xD,
  ASSET_TYPE_GAMEWORLD_SP = 0xE,
  ASSET_TYPE_GAMEWORLD_MP = 0xF,
  ASSET_TYPE_MAP_ENTS = 0x10,
  ASSET_TYPE_GFXWORLD = 0x11,
  ASSET_TYPE_LIGHT_DEF = 0x12,
  ASSET_TYPE_UI_MAP = 0x13,
  ASSET_TYPE_FONT = 0x14,
  ASSET_TYPE_MENULIST = 0x15,
  ASSET_TYPE_MENU = 0x16,
  ASSET_TYPE_LOCALIZE_ENTRY = 0x17,
  ASSET_TYPE_WEAPON = 0x18,
  ASSET_TYPE_WEAPONDEF = 0x19,
  ASSET_TYPE_WEAPON_VARIANT = 0x1A,
  ASSET_TYPE_SNDDRIVER_GLOBALS = 0x1B,
  ASSET_TYPE_FX = 0x1C,
  ASSET_TYPE_IMPACT_FX = 0x1D,
  ASSET_TYPE_AITYPE = 0x1E,
  ASSET_TYPE_MPTYPE = 0x1F,
  ASSET_TYPE_MPBODY = 0x20,
  ASSET_TYPE_MPHEAD = 0x21,
  ASSET_TYPE_CHARACTER = 0x22,
  ASSET_TYPE_XMODELALIAS = 0x23,
  ASSET_TYPE_RAWFILE = 0x24,
  ASSET_TYPE_STRINGTABLE = 0x25,
  ASSET_TYPE_PACK_INDEX = 0x26,
  ASSET_TYPE_XGLOBALS = 0x27,
  ASSET_TYPE_DDL = 0x28,
  ASSET_TYPE_GLASSES = 0x29,
  ASSET_TYPE_EMBLEMSET = 0x2A,
  ASSET_TYPE_COUNT = 0x2B,
  ASSET_TYPE_STRING = 0x2B,
  ASSET_TYPE_ASSETLIST = 0x2C,
};
#include <string>
#include <unordered_map>
std::unordered_map<std::string, BOOL> techniqueConflictCheck;
std::unordered_map<std::string, BOOL> techsetConflictCheck;
std::unordered_map<std::string, BOOL> vsConflictCheck;
std::unordered_map<std::string, BOOL> psConflictCheck;


struct TechsetTechniqueInfo
{
	std::string techniqueName;
	std::vector<int> TechTypes;
};

#include <fstream>
void ListAssetPtrs()
{


	int assetType = 0x06;
	SIZE_T numofbytesread;
	//printf("Listing Assets in the \"%s\" pool.\n\n",getPoolString(assetType));
	//ReadProcessMemory(hProcess,HealthAddress,&buf,sizeof(int),&numofbytesread);
	unsigned int v4 = 0;
	unsigned int assetCount = 0;

	CreateDirectory(L"shader_out",NULL);
	CreateDirectory(L"techset_out",NULL);

	do
	{
		WORD hashbuf = 0;
		ReadProcessMemory(hProcess,db_hashTable+v4,&hashbuf,sizeof(WORD),&numofbytesread);
		if(hashbuf)
		{
			int* NextPoolAddr = 0;
			do
			{
				sPoolPtr PoolPtr1;	

				ReadProcessMemory(hProcess,(BYTE*)AssetList+(hashbuf*sizeof(sPoolPtr)),&PoolPtr1,sizeof(sPoolPtr),&numofbytesread);
				
				if(PoolPtr1.type == assetType)
				{
					char zoneName[80];
					ReadProcessMemory(hProcess,(BYTE*)zoneNameArray + 80*PoolPtr1.zoneID,&zoneName,80,&numofbytesread);
					assetCount++;

					char assetName[80];
					Material mat;
					ReadProcessMemory(hProcess,(BYTE*)PoolPtr1.ptr,&mat,sizeof(mat),&numofbytesread);//PoolPtr1 is the asset

					ReadProcessMemory(hProcess,(BYTE*)mat.info.name,&assetName,80,&numofbytesread);

					MaterialTechniqueSet techset;

					ReadProcessMemory(hProcess,mat.___u8.localTechniqueSet,&techset,sizeof(techset),&numofbytesread);

					//printf("%s\n",assetName,zoneName,PoolPtr1.ptr);

					char name[256];
					ReadProcessMemory(hProcess,techset.name,name,256,&numofbytesread);
					std::string techset_key = name;
			
					if(!techsetConflictCheck.count(techset_key))
					{
						techsetConflictCheck[name] = true;

						std::vector<TechsetTechniqueInfo> TechsetData;
						
						for(int i = 0; i < 130; i++)
						{
							if(techset.techniques[i])
							{
								MaterialTechnique tech;
								ReadProcessMemory(hProcess,techset.techniques[i],&tech,sizeof(tech),&numofbytesread);
								char tech_name[256];
								ReadProcessMemory(hProcess,tech.name,&tech_name,256,&numofbytesread);

								std::string technique_key = tech_name;

								int c = 0;
								for(c = 0; c < TechsetData.size(); c++)
								{
									if(technique_key == TechsetData[c].techniqueName)
									{
										TechsetData[c].TechTypes.push_back(i);
										break;
									}
								}

								if(c >= TechsetData.size())
								{
									TechsetTechniqueInfo tmp;
									tmp.techniqueName = tech_name;
									tmp.TechTypes.push_back(i);
									TechsetData.push_back(tmp);
								}

								
								if(!techniqueConflictCheck.count(technique_key))
								{
									techniqueConflictCheck[technique_key] = true;

									//printf("Technique:	%s\n",tech_name);

									//Do Vertex Shader
									MaterialVertexShader vs;
									ReadProcessMemory(hProcess,tech.passArray[0].vertexShader,&vs,sizeof(vs),&numofbytesread);
									char vs_name[256];
									ReadProcessMemory(hProcess,vs.name,&vs_name,256,&numofbytesread);

									std::string vs_key = vs_name;
									if(!vsConflictCheck.count(vs_key))
									{
										unsigned __int16 shaderSize = vs.prog.loadDef.programSize << 2;
										unsigned __int16 shaderFileSize = shaderSize + 8;//usually its + 8
	
										BYTE* shader_data = new BYTE[shaderSize];
										ReadProcessMemory(hProcess,vs.prog.loadDef.program,shader_data,shaderSize,&numofbytesread);
	
										std::string filepath = "shader_out/";
										filepath += "vs_main_vs_3_0_"; //vertex shaders use this
										filepath += vs_name;

										if(!FileExists(filepath.c_str()) || overwriteFiles) //prevents collisions even more
										{
											printf("Vertex Shader:	%s\n",vs_name);
											std::ofstream ofile(filepath.c_str(), std::ios::out | std::ios::binary);
			
											DWORD NULL_DATA = 0;
											ofile.write((char*)&NULL_DATA,sizeof(DWORD));
											ofile.write((char*)&shaderSize,sizeof(unsigned __int16));
											ofile.write((char*)&NULL_DATA,sizeof(unsigned __int16));
								
											ofile.write((char*)shader_data,shaderSize);
											ofile.close();
											delete[] shader_data;
										}
									}

									//Do Pixel Shader
									MaterialPixelShader ps;
									ReadProcessMemory(hProcess,tech.passArray[0].___u2.pixelShader,&ps,sizeof(ps),&numofbytesread);
									char ps_name[256];
									ReadProcessMemory(hProcess,ps.name,&ps_name,256,&numofbytesread);

									std::string ps_key = ps_name;
									if(!psConflictCheck.count(ps_key))
									{
										unsigned __int16 shaderSize = ps.prog.loadDef.programSize << 2;
										unsigned __int16 shaderFileSize = shaderSize + 8;//usually its + 8
	
										BYTE* shader_data = new BYTE[shaderSize];
										ReadProcessMemory(hProcess,ps.prog.loadDef.program,shader_data,shaderSize,&numofbytesread);

										std::string filepath = "shader_out/";
										filepath += "ps_main_ps_3_0_"; //Pixel shaders use this
										filepath += ps_name;

										if(!FileExists(filepath.c_str()) || overwriteFiles) //prevents collisions even more TEMPORARY
										{
											printf("Pixel Shader:	%s\n",ps_name);
											std::ofstream ofile(filepath.c_str(), std::ios::out | std::ios::binary);
			
											DWORD NULL_DATA = 0;
											ofile.write((char*)&NULL_DATA,sizeof(DWORD));
											ofile.write((char*)&shaderSize,sizeof(unsigned __int16));
											ofile.write((char*)&NULL_DATA,sizeof(unsigned __int16));
								
											ofile.write((char*)shader_data,shaderSize);
											ofile.close();
											delete[] shader_data;
										}
									}
								}
							}
						}

						std::string filepath = "techset_out/";
						filepath += techset_key;
						filepath += ".techset";

						if(!FileExists(filepath.c_str()) || overwriteFiles)
						{
							printf("Techset:	%s\n",name);
							std::ofstream ofile(filepath, std::ios::out | std::ios::binary);
							for(int i = 0; i < TechsetData.size(); i++)
							{
								std::string outBuf;
								for(int t = 0; t < TechsetData[i].TechTypes.size(); t++)
								{
									outBuf += Material_TechniqueTypeForName(TechsetData[i].TechTypes[t]);
									outBuf += ":\r\n";
								}
								outBuf += "	";
								outBuf += TechsetData[i].techniqueName;
								outBuf += ";\r\n\r\n";
								ofile.write(outBuf.c_str(),outBuf.size());
							}
					
							ofile.close();
						}
					}

					int b = sizeof(sPoolPtr);
				}
				NextPoolAddr = 0;	
			}
			while(NextPoolAddr);
		}
		v4 += 2;
	}
	while (v4 < 0x10000);
}

int main(int argc, char** argv)
{
	if(argc > 1)
	{
		overwriteFiles = true;
	}

	//freopen("out.txt","w",stdout);
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry) == TRUE)
	{
		while (Process32Next(snapshot, &entry) == TRUE && hProcess == nullptr)
		{
			//wprintf(entry.szExeFile);
			//printf("\n\n");
			if (_wcsicmp(entry.szExeFile, L"BlackOps.exe") == 0)
			{  
				printf("Found BlackOpsSP\n\n");
				hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
				ListAssetPtrs();
			}
			else if (_wcsicmp(entry.szExeFile, L"BlackOpsMP.exe") == 0)
			{
				db_hashTable = (BYTE*)0x24365D8;
				AssetList = (sPoolPtr*)0x252FC18;
				zoneNameArray = (char*)0x028C9B08;

				printf("Found BlackOpsMP\n\n");
				hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
				ListAssetPtrs();
			}
		}
	}
	
	if(hProcess)
	{
		CloseHandle(hProcess);
	}
	else
	{
		printf("Error: Could not find BlackOps or BlackOpsMP process\n");
		return 1;
	}
	
	return 0;
};