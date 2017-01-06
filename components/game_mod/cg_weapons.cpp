#include "stdafx.h"

// /cgame/cg_weapons.cpp:3329
void CG_BulletEndpos(unsigned int *randSeed, const float spread, const float *start, float *end, float *dir, const float *forwardDir, const float *rightDir, const float *upDir, const float maxRange, int shotIndex, int maxShotIndex)
{
	ASSERT(!IS_NAN(spread));
	ASSERT(end);

	float aimOffset = tan(spread * 0.017453292f) * maxRange;

	ASSERT(!IS_NAN(aimOffset));

	float right = 0.0f;
	float up = 0.0f;
	Bullet_RandomDir(randSeed, &right, &up);

	// BO2 weapon perk: bullet spread is horizontal only
	if (Com_SessionMode_IsZombiesGame() && /* BG_WeaponHasPerk(wp->weapDef, 3) */ false)
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

	ASSERT(!IS_NAN(right));
	ASSERT(!IS_NAN(up));

	end[0] = (maxRange * forwardDir[0]) + start[0];
	end[1] = (maxRange * forwardDir[1]) + start[1];
	end[2] = (maxRange * forwardDir[2]) + start[2];

	ASSERT(!IS_NAN(end[0]) && !IS_NAN(end[1]) && !IS_NAN(end[2]));

	end[0] = (right * rightDir[0]) + end[0];
	end[1] = (right * rightDir[1]) + end[1];
	end[2] = (right * rightDir[2]) + end[2];

	end[0] = (up * upDir[0]) + end[0];
	end[1] = (up * upDir[1]) + end[1];
	end[2] = (up * upDir[2]) + end[2];

	ASSERT(!IS_NAN(end[0]) && !IS_NAN(end[1]) && !IS_NAN(end[2]));

	if (dir)
	{
		dir[0] = end[0] - start[0];
		dir[1] = end[1] - start[1];
		dir[2] = end[2] - start[2];
		Vec3Normalize(dir);

		ASSERT(!IS_NAN(dir[0]) && !IS_NAN(dir[1]) && !IS_NAN(dir[2]));
	}
}

// /cgame/cg_weapons.cpp:3486
void __declspec(naked) mfh_CG_DrawBulletImpacts1()
{
	static DWORD dwJmp = 0x0079A4F7;

	__asm
	{
		// [shotCount,    SP - 0x114]
		// [secondBarrel, SP - 0xE0]
		// [ps,           SP + 0x10]
		pushad
		lea edi, [esp + 0x150 + 0x20]

		lea eax, [edi - 0xE0]			// secondBarrel [See mfh_CG_DrawBulletImpacts2 comment]

		mov ebp, dword ptr [edi + 0x10]	// playerState *ps
		mov edx, dword ptr [ebp]		//
		mov dword ptr [eax], edx		// *randSeed = ps->commandTime
		push eax						//
		call BG_seedRandWithGameTime	// BG_seedRandWithGameTime(randSeed) as in BO2
		add esp, 0x4					//

		push PERK_DOUBLETAP				// PERK_DOUBLETAP index
		lea ebp, [ebp + 0x4FC]			//
		push ebp						// ps->perks
		call BG_HasPerk					//
		add esp, 0x8					//

		test al, al						// Check if player had perk
		je __noperk						//
		shl dword ptr [edi - 0x114], 1	// shotCount *= 2

__noperk:
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