#pragma once

struct BulletFireParams
{
	int weaponEntIndex;
	int ignoreEntIndex;
	float damageMultiplier;
	int methodOfDeath;
	float origStart[3];
	float start[3];
	float end[3];
	float dir[3];
	char _pad0[4];
};
STATIC_ASSERT_OFFSET(BulletFireParams, weaponEntIndex, 0x0);
STATIC_ASSERT_OFFSET(BulletFireParams, ignoreEntIndex, 0x4);
STATIC_ASSERT_OFFSET(BulletFireParams, damageMultiplier, 0x8);
STATIC_ASSERT_OFFSET(BulletFireParams, methodOfDeath, 0xC);
STATIC_ASSERT_OFFSET(BulletFireParams, origStart, 0x10);
STATIC_ASSERT_OFFSET(BulletFireParams, start, 0x1C);
STATIC_ASSERT_OFFSET(BulletFireParams, end, 0x28);
STATIC_ASSERT_OFFSET(BulletFireParams, dir, 0x34);
STATIC_ASSERT_SIZE(BulletFireParams, 0x44);

float G_GoodRandomFloat(int *idum);
void Bullet_RandomDir(unsigned int *randSeed, float *x, float *y);
void Bullet_Endpos(unsigned int *randSeed, float spread, float *end, float *dir, weaponParms *wp, float maxRange, int shotIndex, int maxShotIndex);
void Bullet_FireExtended(BulletFireParams *params, WeaponVariantDef *weapVariantDef, gentity_s *attacker, int gameTime);
void Bullet_FirePenetrate(BulletFireParams *params, WeaponVariantDef *weapVariantDef, gentity_s *attacker, int gameTime);
bool BG_WeaponBulletFire_ExplodeOnImpact(WeaponDef *weapDef);
bool BG_WeaponBulletFire_ShouldPenetrate(WeaponDef *weapDef);
void Bullet_Fire(gentity_s *attacker, float spread, weaponParms *wp, gentity_s *weaponEnt, int gameTime);

void BG_seedRandWithGameTime(unsigned int *pHoldrand);
void BG_srand(unsigned int *pHoldrand);
double BG_random(unsigned int *pHoldrand);