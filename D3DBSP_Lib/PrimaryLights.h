#include "Lump.h"

enum GfxLightType
{
  GFX_LIGHT_TYPE_NONE = 0x0,
  GFX_LIGHT_TYPE_DIR = 0x1,
  GFX_LIGHT_TYPE_SPOT = 0x2,
  GFX_LIGHT_TYPE_OMNI = 0x3,
  GFX_LIGHT_TYPE_COUNT = 0x4,
  GFX_LIGHT_TYPE_DIR_SHADOWMAP = 0x4,
  GFX_LIGHT_TYPE_SPOT_SHADOWMAP = 0x5,
  GFX_LIGHT_TYPE_OMNI_SHADOWMAP = 0x6,
  GFX_LIGHT_TYPE_COUNT_WITH_SHADOWMAP_VERSIONS = 0x7,
};

struct DiskPrimaryLight_BO
{
  char type;
  char canUseShadowMap;
  __int16 cullDist;
  char priority;
  char _pad[3];
  float dir[3];
  float origin[3];
  float color[3];
  float specularcolor[3];
  float attenuation[3];
  float angle[3];
  float bouncecolor[3];
  float intensity;
  float cutOn;
  float radius;
  float nearEdge;
  float farEdge;
  float cosHalfFovOuter;
  float cosHalfFovInner;
  int exponent;
  float rotationLimit;
  float translationLimit;
  float roundness;
  float mipDistance;
  float aAbB[4];
  float cookieControl0[4];
  float cookieControl1[4];
  float cookieControl2[4];
  char defName[64];
};

struct DiskPrimaryLight_WAW
{
	BYTE type;
	char canUseShadowMap;
	WORD cullDist;
	char priority;
	float color[3];
	float dir[3];
	float origin[3];
	float radius; 
	float cosHalfFovOuter;//cos(fovouter/2) from radiant
	float cosHalfFovInner; //cos(fovinner/2) from radiant
	float exponent;	//the exponent integer kv in radiant
	float rotationLimit; //cos(maxturn (deg)) in radiant
	float translationLimit; //maxmove in radiant
	char defName[64];
};

int ConvertLump_WAWtoBO_PrimaryLights(Lump* wawLump, Lump* boLump);
int ConvertLump_BOtoWAW_PrimaryLights(Lump* boLump, Lump* wawLump);