#pragma once

typedef int DObj;
typedef int XAnimTree_s;
typedef int XAnim_s;

struct ViewModelHand
{
	int iPrevAnim;
	int iHandAnimIndex;
};
STATIC_ASSERT_OFFSET(ViewModelHand, iPrevAnim, 0x0);
STATIC_ASSERT_OFFSET(ViewModelHand, iHandAnimIndex, 0x4);

struct ViewModelInfo
{
	DObj *viewModelDObj;
	int hasAnimTree;
	XAnimTree_s *tree;
	XAnim_s *anims;
	unsigned int partBits[5];
	ViewModelHand hand[2];
};
STATIC_ASSERT_OFFSET(ViewModelInfo, anims, 0xC);
STATIC_ASSERT_OFFSET(ViewModelInfo, hand, 0x24);

extern void (__cdecl * CL_Vid_Restart_Complete_f)();

void hk_CL_Vid_Restart_Complete_f();
void CL_CM_LoadMap(const char *mapname);

typedef const char *(__cdecl* CL_GetServerIPAddress_t)();
extern CL_GetServerIPAddress_t CL_GetServerIPAddress_o;

const char *__cdecl CL_GetServerIPAddress();

char *CG_GetLocalClientGlobals(int localClientNum);
ViewModelInfo *CG_GetLocalClientViewModelInfo(int localClientNum);