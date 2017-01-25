#include "stdafx.h"

// /game/bullet.cpp:54
float G_GoodRandomFloat(int *idum)
{
	// http://www.biostat.umn.edu/~cavanr/random.c ran1()
	#define IA 16807                
	#define IM 2147483647
	#define AM (1.0/IM)
	#define IQ 127773
	#define IR 2836
	#define NTAB 32
	#define NDIV (1+(IM-1)/NTAB)
	#define EPS 1.2e-7
	#define RNMX (1.0-EPS)

	long k;
	int iv[NTAB];

	*idum = -*idum;

	for (int j = NTAB + 7; j >= 0; j--)
	{
		k = (*idum) / IQ;
		*idum = IA * (*idum - k * IQ) - IR * k;

		if (*idum < 0)
			*idum += IM;

		if (j < NTAB)
			iv[j] = *idum;
	}

	k = (*idum) / IQ;
	*idum = IA * (*idum - k * IQ) - IR * k;

	if (*idum < 0)
		*idum += IM;

	if (RNMX - iv[iv[0] / NDIV] * 4.656612875245797e-10 < 0.0)
		return (float)(RNMX);

	return (float)(iv[iv[0] / NDIV] * 4.656612875245797e-10);
}

// /game/bullet.cpp:85
void Bullet_RandomDir(unsigned int *randSeed, float *x, float *y)
{
	ASSERT(x);
	ASSERT(y);

	//
	// Original code before swapping to BO2:
	// float theta = G_GoodRandomFloat(&time) * 360.0;
	// float r = G_GoodRandomFloat(&time);
	//
	double theta	= BG_random(randSeed) * 360.0;
	BG_srand(randSeed);
	double r		= BG_random(randSeed);
	double radians	= (theta * 0.017453292); // PI / 180.0

	*x = (float)(r * cos(radians));
	*y = (float)(r * sin(radians));
}

// /game/bullet.cpp:106
void Bullet_Endpos(unsigned int *randSeed, float spread, float *end, float *dir, weaponParms *wp, float maxRange, int shotIndex, int maxShotIndex)
{
	ASSERT(!IS_NAN(spread));
	ASSERT(end);
	ASSERT(wp);

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

	ASSERT(!IS_NAN(wp->muzzleTrace[0]) && !IS_NAN(wp->muzzleTrace[1]) && !IS_NAN(wp->muzzleTrace[2]));
	ASSERT(!IS_NAN(wp->forward[0]) && !IS_NAN(wp->forward[1]) && !IS_NAN(wp->forward[2]));
	ASSERT(!IS_NAN(wp->right[0]) && !IS_NAN(wp->right[1]) && !IS_NAN(wp->right[2]));
	ASSERT(!IS_NAN(wp->up[0]) && !IS_NAN(wp->up[1]) && !IS_NAN(wp->up[2]));

	end[0] = (maxRange * wp->forward[0]) + wp->muzzleTrace[0];
	end[1] = (maxRange * wp->forward[1]) + wp->muzzleTrace[1];
	end[2] = (maxRange * wp->forward[2]) + wp->muzzleTrace[2];

	ASSERT(!IS_NAN(end[0]) && !IS_NAN(end[1]) && !IS_NAN(end[2]));

	end[0] = (right * wp->right[0]) + end[0];
	end[1] = (right * wp->right[1]) + end[1];
	end[2] = (right * wp->right[2]) + end[2];

	end[0] = (up * wp->up[0]) + end[0];
	end[1] = (up * wp->up[1]) + end[1];
	end[2] = (up * wp->up[2]) + end[2];

	ASSERT(!IS_NAN(end[0]) && !IS_NAN(end[1]) && !IS_NAN(end[2]));

	if (dir)
	{
		dir[0] = end[0] - wp->muzzleTrace[0];
		dir[1] = end[1] - wp->muzzleTrace[1];
		dir[2] = end[2] - wp->muzzleTrace[2];
		Vec3Normalize(dir);

		ASSERT(!IS_NAN(dir[0]) && !IS_NAN(dir[1]) && !IS_NAN(dir[2]));
	}
}

// /game/bullet.cpp:655
void Bullet_FireExtended(BulletFireParams *params, WeaponVariantDef *weapVariantDef, gentity_s *attacker, int gameTime)
{
	static DWORD dwCall = 0x007D42F0;

	__asm
	{
		push gameTime
		push attacker
		mov esi, weapVariantDef
		mov edi, params
		call [dwCall]
		add esp, 0x8
	}
}

// /game/bullet.cpp:721
void Bullet_FirePenetrate(BulletFireParams *params, WeaponVariantDef *weapVariantDef, gentity_s *attacker, int gameTime)
{
	static DWORD dwCall = 0x007D3CD0;

	__asm
	{
		push gameTime
		push attacker
		push weapVariantDef
		mov esi, params
		call [dwCall]
		add esp, 0xC
	}
}

// /game/bullet.cpp:869
bool BG_WeaponBulletFire_ExplodeOnImpact(WeaponDef *weapDef)
{
	ASSERT(weapDef);
	ASSERT(weapDef->weapType == WEAPTYPE_BULLET);

	return weapDef->bBulletImpactExplode || bg_forceExplosiveBullets->current.enabled;
}

// /game/bullet.cpp:880
bool BG_WeaponBulletFire_ShouldPenetrate(WeaponDef *weapDef)
{
	ASSERT(weapDef);
	ASSERT(weapDef->weapType == WEAPTYPE_BULLET);

	if (weapDef->penetrateType != PENETRATE_TYPE_NONE)
		return BG_WeaponBulletFire_ExplodeOnImpact(weapDef) == false;

	return false;
}

// /game/bullet.cpp:897
void Bullet_Fire(gentity_s *attacker, float spread, weaponParms *wp, gentity_s *weaponEnt, int gameTime)
{
	ASSERT(attacker);
	ASSERT(wp);
	ASSERT(wp->weapDef);
	ASSERT(wp->weapDef->weapType == WEAPTYPE_BULLET);

	DWORD v22 = 0;
	int shotCount = 1;
	float range = sv_bullet_range->current.value;

	if (wp->weapDef->weapClass == WEAPCLASS_SPREAD)
	{
		shotCount = wp->weapDef->shotCount;
		range = wp->weapDef->fMinDamageRange;
	}

	if (attacker->client)
	{
		DWORD v11 = *(DWORD *)((DWORD)attacker->client + 0x1C84);

		if (v11)
		{
			v22 = *(DWORD *)(v11 + 0x100);
			*(DWORD *)(v11 + 0x100) = 0;
		}

		// Used for various stat tracking (GSC/mystery box)
		BG_SetWeaponUsed(attacker->client->ps.clientNum, BG_GetWeaponIndex(wp->weapVariantDef));
	}

	// BO2 double-tap modification
	if (Com_SessionMode_IsZombiesGame() && perk_weapRateEnhanced->current.enabled && attacker->s.eType == 1)
	{
		if (BG_HasPerk(attacker->client->ps.perks, PERK_RATEOFFIRE))
			shotCount *= 2;
	}

	unsigned int randSeed = gameTime;
	BG_seedRandWithGameTime(&randSeed);

	for (int shotIndex = 0; shotIndex < shotCount; shotIndex++)
	{
		BulletFireParams fireParams;
		memset(&fireParams, 0, sizeof(fireParams));

		if (weaponEnt)
		{
			fireParams.weaponEntIndex = weaponEnt->s.number;
			fireParams.ignoreEntIndex = weaponEnt->s.number;
		}
		else
		{
			fireParams.weaponEntIndex = 1022;
			fireParams.ignoreEntIndex = 1022;
		}

		if (vehicle_selfCollision->current.enabled && weaponEnt && weaponEnt->vehicle)
			fireParams.ignoreEntIndex = 1022;

		fireParams.damageMultiplier = 1.0f;
		fireParams.methodOfDeath = (wp->weapDef->bRifleBullet != 0) + 1;
		fireParams.origStart[0] = wp->muzzleTrace[0];
		fireParams.origStart[1] = wp->muzzleTrace[1];
		fireParams.origStart[2] = wp->muzzleTrace[2];
		fireParams.start[0] = wp->muzzleTrace[0];
		fireParams.start[1] = wp->muzzleTrace[1];
		fireParams.start[2] = wp->muzzleTrace[2];
		Bullet_Endpos(&randSeed, spread, fireParams.end, fireParams.dir, wp, range, shotIndex, shotCount);

		if (g_debugBullets->current.integer == 1)
			CG_DebugLine(fireParams.start, fireParams.end, (float *)0x00A5E834, 1, 1000);

		if (BG_WeaponBulletFire_ShouldPenetrate(wp->weapDef))
			Bullet_FirePenetrate(&fireParams, wp->weapVariantDef, attacker, gameTime);
		else
			Bullet_FireExtended(&fireParams, wp->weapVariantDef, attacker, gameTime);
	}
}

void BG_seedRandWithGameTime(unsigned int *pHoldrand)
{
	unsigned int v1 = *pHoldrand << 8;
	*pHoldrand ^= v1;
	*pHoldrand ^= (v1 << 8) ^ (v1 << 16);
}

void BG_srand(unsigned int *pHoldrand)
{
	*pHoldrand = 214013 * (214013 * (214013 * (214013 * *pHoldrand + 2531011) + 2531011) + 2531011) + 2531011;
}

double BG_random(unsigned int *pHoldrand)
{
	*pHoldrand = 214013 * *pHoldrand + 2531011;
	return (*pHoldrand >> 17) * (1.0 / 32768.0);
}