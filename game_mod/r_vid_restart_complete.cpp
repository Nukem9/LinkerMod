#include "stdafx.h"
#include "r_vid_restart_complete.h"

enum dvarType_t
{
  DVAR_TYPE_BOOL = 0x0,
  DVAR_TYPE_FLOAT = 0x1,
  DVAR_TYPE_FLOAT_2 = 0x2,
  DVAR_TYPE_FLOAT_3 = 0x3,
  DVAR_TYPE_FLOAT_4 = 0x4,
  DVAR_TYPE_INT = 0x5,
  DVAR_TYPE_ENUM = 0x6,
  DVAR_TYPE_STRING = 0x7,
  DVAR_TYPE_COLOR = 0x8,
  DVAR_TYPE_INT64 = 0x9,
  DVAR_TYPE_LINEAR_COLOR_RGB = 0xA,
  DVAR_TYPE_COLOR_XYZ = 0xB,
  DVAR_TYPE_COUNT = 0xC,
};

union DvarValue
{
  bool enabled;
  int integer;
  unsigned int unsignedInt;
  __int64 integer64;
  unsigned __int64 unsignedInt64;
  float value;
  float vector[4];
  const char *string;
  char color[4];
};

/* 1139 */
union DvarLimits
{
  struct
  {
		int stringCount;
		const char **strings;
  } enumeration;
  
  struct
  {
		int min;
		int max;
  } integer;
  
  struct
  {
		__int64 min;
		__int64 max;
  } integer64;
  
  struct
  {
		float min;
		float max;
  } value;

  struct
  {
		float min;
		float max;
  } vector;
};

struct dvar_s
{
  const char *name;
  const char *description;
  int hash;
  unsigned int flags;
  dvarType_t type;
  bool modified;
  bool loadedFromSaveGame;
  DvarValue current;
  DvarValue latched;
  DvarValue reset;
  DvarValue saved;
  DvarLimits domain;
  dvar_s *hashNext;
};

static dvar_s** p_com_sv_running = ((dvar_s**)0x0243FD3C);

void CL_Vid_Restart_Complete_f()
{
	if((*p_com_sv_running)->current.enabled)
	{
		SV_KillServer_f();
		pCL_Vid_Restart_Complete_f();
	}
	else
	{
		pCL_Vid_Restart_Complete_f();
	}
	//possible map_restart (frontend fix)
}