#include "stdafx.h"

void XAnimSetTime(XAnimTree_s *tree, unsigned int animIndex, float time, int cmdIndex)
{
	ASSERT(tree);
	//ASSERT(tree->anims);
	//ASSERT(animIndex < tree->anims->size);

	((void(__cdecl *)(XAnimTree_s *, unsigned int, float, int))0x005E79C0)(tree, animIndex, time, cmdIndex);
}

int XAnimSetGoalWeight(DObj *obj, unsigned int animIndex, float goalWeight, float goalTime, float rate, unsigned int notifyName, unsigned int notifyType, int bRestart, int cmdIndex)
{
	ASSERT(obj);

	return ((int(__cdecl *)(DObj *, unsigned int, float, float, float, unsigned int, unsigned int, int, int))0x004076C0)
		(obj, animIndex, goalWeight, goalTime, rate, notifyName, notifyType, bRestart, cmdIndex);
}

// /cgame/cg_weapons.cpp:264
float GetWeaponAnimRate(int localClientNum, WeaponVariantDef *weapVariantDef, XAnim_s *anims, int animIndex)
{
	static DWORD dwCall = 0x00795040;

	ASSERT(weapVariantDef);
	ASSERT(anims);

	__asm
	{
		push anims
		push weapVariantDef
		mov esi, animIndex
		mov eax, localClientNum
		call[dwCall]
		add esp, 0x8

		// Return value in both xmm0 and ST(0)
		movd eax, xmm0
		push eax
		fld [esp]
		add esp, 0x4
	}
}

// /cgame/cg_weapons.cpp:298
float GetWeaponAnimTimeFrac(int localClientNum, WeaponVariantDef *weapVariantDef, XAnim_s *anims, int animIndex)
{
	static DWORD dwCall = 0x007950F0;

	ASSERT(weapVariantDef);
	ASSERT(anims);

	__asm
	{
		mov edx, weapVariantDef
		mov eax, animIndex
		call[dwCall]

		// Return value in both xmm0 and ST(0)
		movd eax, xmm0
		push eax
		fld[esp]
		add esp, 0x4
	}
}

// /cgame/cg_weapons.cpp:373
void StartWeaponAnim(int localClientNum, int weaponNum, DObj *obj, int animIndex, float transitionTime, int newPlayerstate)
{
	ASSERT((animIndex > WEAP_ANIM_VIEWMODEL_START) && (animIndex < WEAP_ANIM_VIEWMODEL_END));

	ViewModelInfo *viewModelInfo = CG_GetLocalClientViewModelInfo(localClientNum);
	XAnim_s *anims = viewModelInfo->anims;

	ASSERT(anims);

	WeaponDef *weapDef = BG_GetWeaponDef(weaponNum);
	WeaponVariantDef *weapVariantDef = BG_GetWeaponVariantDef(weaponNum);

	float rate = GetWeaponAnimRate(localClientNum, weapVariantDef, anims, animIndex);
	float timeFrac = 0.0f;

	if (newPlayerstate)
		timeFrac = GetWeaponAnimTimeFrac(localClientNum, weapVariantDef, anims, animIndex);

	char *cgameGlob = CG_GetLocalClientGlobals(localClientNum);

	unsigned int *ps_perks	= (unsigned int *)(cgameGlob + 0x8A860);// perks
	int weaponstate			= *(int *)(cgameGlob + 0x8A4BC);		// cg->predictedPlayerState.weaponstate
	int weaponstateLeft		= *(int *)(cgameGlob + 0x8A4C0);		// cg->predictedPlayerState.weaponstateLeft

	if ((IS_WEAPONSTATE_RELOAD(weaponstate) || IS_WEAPONSTATE_RELOAD(weaponstateLeft)) && BG_HasPerk(ps_perks, PERK_FASTRELOAD))
	{
		// Fast weapon reload perk
		if (perk_weapReloadMultiplier->current.value != 0.0f)
			rate /= perk_weapReloadMultiplier->current.value;
		else
			rate = 1000.0f;
	}
	else if ((weaponstate == WEAPON_RECHAMBER_START || weaponstate == WEAPON_RECHAMBERING) && BG_HasPerk(ps_perks, PERK_RATEOFFIRE))
	{
		// Doubletap (rate of fire) perk
		if (perk_weapRateMultiplier->current.value != 0.0f)
			rate /= perk_weapRateMultiplier->current.value;
		else
			rate = 1000.0f;
	}
	else if (BG_CanFastSwitch(weapDef, weaponstate) && BG_HasPerk(ps_perks, PERK_FASTSWITCH))
	{
		// Fast weapon switch perk
		ASSERT(perk_weapSwitchMultiplier->current.value > 0.0f);
		rate /= perk_weapSwitchMultiplier->current.value;
	}
	else if (IS_WEAPONSTATE_OFFHAND(weaponstate) && BG_HasPerk(ps_perks, PERK_FASTOFFHAND))
	{
		// Fast offhand throw perk
		ASSERT(perk_fastOffhandRate->current.value > 0.0f);
		rate /= perk_fastOffhandRate->current.value;
	}
	else if (weaponstate == WEAPON_SPRINT_DROP && BG_HasPerk(ps_perks, PERK_FASTSPRINTRECOVERY))
	{
		// Fast sprint recovery perk
		ASSERT(perk_fastSprintRate->current.value > 0.0f);
		rate /= perk_fastSprintRate->current.value;
	}
	else if (IS_WEAPONSTATE_MELEE(weaponstate) && BG_HasPerk(ps_perks, PERK_FASTMELEE))
	{
		// Fast melee perk
		ASSERT(perk_fastMeleeRate->current.value > 0.0f);
		rate /= perk_fastMeleeRate->current.value;
	}

	for (int i = 1; i < WEAP_ANIM_VIEWMODEL_END; i++)
	{
		if (animIndex == i)
		{
			XAnimSetGoalWeight(obj, i, 1.0f, transitionTime, rate, 0, 1, 1, -1);

			if (newPlayerstate)
			{
				XAnimSetTime(viewModelInfo->tree, i, timeFrac, -1);
				//ASSERT(viewModelInfo->tree->anims);
			}
		}
		else if (weapDef->bDualWield
			&& viewModelInfo->hand[0].iHandAnimIndex != i
			&& viewModelInfo->hand[1].iHandAnimIndex != i)
		{
			XAnimSetGoalWeight(obj, i, 0.0f, transitionTime, 1.0f, 0, 0, 0, -1);
		}
		else if (!weapDef->bDualWield)
		{
			XAnimSetGoalWeight(obj, i, 0.0f, transitionTime, 1.0f, 0, 0, 0, -1);
		}
	}
}

void hk_StartWeaponAnim(int localClientNum, DObj *obj, int animIndex, float transitionTime, int newPlayerstate)
{
	__asm
	{
		push newPlayerstate
		push transitionTime
		push animIndex
		push obj
		push eax
		push localClientNum
		call StartWeaponAnim
		add esp, 0x18
	}
}

// /cgame/cg_weapons.cpp:3329
void CG_BulletEndpos(unsigned int *randSeed, const float spread, const float *start, float *end, float *dir, const float *forwardDir, const float *rightDir, const float *upDir, const float maxRange, int shotIndex, int maxShotIndex)
{
	DBG_ASSERT(!IS_NAN(spread));
	DBG_ASSERT(end);

	float aimOffset = tan(spread * 0.017453292f) * maxRange;

	DBG_ASSERT(!IS_NAN(aimOffset));

	float right = 0.0f;
	float up = 0.0f;
	Bullet_RandomDir(randSeed, &right, &up);

	// BO2 weapon perk: bullet spread is horizontal only
	if (Com_SessionMode_IsZombiesGame() && BG_WeaponHasPerk(nullptr, 3))
	{
		right = 0.0f;
		up = 0.0f;

		if (shotIndex && maxShotIndex > 1)
		{
			float modifier = (shotIndex % 2) ? -1.0f : 1.0f;
			right = (((spread * 0.5f) / (maxShotIndex >> 1)) * (((shotIndex - 1) >> 1) + 1)) * modifier;
		}
	}

	right *= aimOffset;
	up *= aimOffset;

	DBG_ASSERT(!IS_NAN(right));
	DBG_ASSERT(!IS_NAN(up));

	end[0] = (maxRange * forwardDir[0]) + start[0];
	end[1] = (maxRange * forwardDir[1]) + start[1];
	end[2] = (maxRange * forwardDir[2]) + start[2];

	DBG_ASSERT(!IS_NAN(end[0]) && !IS_NAN(end[1]) && !IS_NAN(end[2]));

	end[0] = (right * rightDir[0]) + end[0];
	end[1] = (right * rightDir[1]) + end[1];
	end[2] = (right * rightDir[2]) + end[2];

	end[0] = (up * upDir[0]) + end[0];
	end[1] = (up * upDir[1]) + end[1];
	end[2] = (up * upDir[2]) + end[2];

	DBG_ASSERT(!IS_NAN(end[0]) && !IS_NAN(end[1]) && !IS_NAN(end[2]));

	if (dir)
	{
		dir[0] = end[0] - start[0];
		dir[1] = end[1] - start[1];
		dir[2] = end[2] - start[2];
		Vec3Normalize(dir);

		DBG_ASSERT(!IS_NAN(dir[0]) && !IS_NAN(dir[1]) && !IS_NAN(dir[2]));
	}
}

// /cgame/cg_weapons.cpp:3486
void __declspec(naked) mfh_CG_DrawBulletImpacts1()
{
	static DWORD dwJmp = 0x0079A4F7;

	playerState_s *ps;
	unsigned int *randSeed;
	unsigned int *shotCount;

	__asm
	{
		// [shotCount,    SP - 0x114]
		// [secondBarrel, SP - 0xE0]
		// [ps,           SP + 0x10]
		pushad
		lea edi, [esp + 0x150 + 0x20]

		mov ebp, esp
		sub esp, __LOCAL_SIZE

		mov eax, dword ptr [edi + 0x10]	// playerState *ps
		mov ps, eax						//
		lea eax, [edi - 0xE0]			// secondBarrel [See mfh_CG_DrawBulletImpacts2 comment]
		mov randSeed, eax				//
		lea eax, [edi - 0x114]			// shotCount
		mov shotCount, eax				//
	}

	*randSeed = ps->commandTime;
	BG_seedRandWithGameTime(randSeed);

	if (Com_SessionMode_IsZombiesGame() && perk_weapRateEnhanced->current.enabled)
	{
		if (BG_HasPerk(ps->perks, PERK_RATEOFFIRE))
			*shotCount *= 2;
	}

	__asm
	{
		mov esp, ebp					//
		popad							// Restore stack
		xor ebx, ebx					// Original instructions
		cmp dword ptr [esp + 0x3C], ebx	//
		jmp [dwJmp]						//
	}
}

void __declspec(naked) mfh_CG_DrawBulletImpacts2()
{
	static DWORD dwJmp = 0x0079A5BA;

	__asm
	{
		// [shotIndex,    ebx]
		// [spread,       SP - 0x130]
		// [maxRange,     SP - 0x118]
		// [shotCount,    SP - 0x114]
		// [forwardDir,   SP - 0x104]
		// [rightDir,     SP - 0xF8]
		// [upDir,        SP - 0xEC]
		// [secondBarrel, SP - 0xE0]
		// [start,        SP - 0xA8]
		// [end,          SP - 0x9C]
		// [dir,          SP - 0x90]
		// [ps,           SP + 0x10]
		lea edi, [esp + 0x168]

		// Call CG_BulletEndpos with completely rewritten arguments
		push dword ptr [edi - 0x114]	// shotCount
		push ebx						// shotIndex
		push dword ptr [edi - 0x118]	// maxRange
		lea eax, [edi - 0xEC]			// &upDir
		push eax						//
		lea eax, [edi - 0xF8]			// &rightDir
		push eax						//
		lea eax, [edi - 0x104]			// &forwardDir
		push eax						//
		lea eax, [edi - 0x90]			// &dir
		push eax						//
		lea eax, [edi - 0x9C]			// &end
		push eax						//
		lea eax, [edi - 0xA8]			// &start
		push eax						//
		push dword ptr [edi - 0x130]	// spread
		lea eax, [edi - 0xE0]			// secondBarrel [NOTE: Hijacked to instead be a pointer as 'randSeed']
		push eax						//

		call CG_BulletEndpos
		add esp, 0x2C

		jmp [dwJmp]
	}
}