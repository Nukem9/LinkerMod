#pragma once
#include "dvar.h"

typedef dvar_s* (*CG_RegisterScoreboardDvars_t)();
extern CG_RegisterScoreboardDvars_t CG_RegisterScoreboardDvars_o;

extern dvar_s* cg_showServerInfo;

dvar_s* CG_RegisterScoreboardDvars();
void mfh_CG_DrawBackdropServerInfo();
