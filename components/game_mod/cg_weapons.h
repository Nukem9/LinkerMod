#pragma once

#define WEAP_ANIM_VIEWMODEL_START 0
#define WEAP_ANIM_VIEWMODEL_END 64

float GetWeaponAnimRate(int localClientNum, WeaponVariantDef *weapVariantDef, XAnim_s *anims, int animIndex);
float GetWeaponAnimTimeFrac(int localClientNum, WeaponVariantDef *weapVariantDef, XAnim_s *anims, int animIndex);
void StartWeaponAnim(int localClientNum, int weaponNum, DObj *obj, int animIndex, float transitionTime, int newPlayerstate);
void hk_StartWeaponAnim(int localClientNum, DObj *obj, int animIndex, float transitionTime, int newPlayerstate);
void CG_BulletEndpos(unsigned int *randSeed, const float spread, const float *start, float *end, float *dir, const float *forwardDir, const float *rightDir, const float *upDir, const float maxRange, int shotIndex, int maxShotIndex);
void mfh_CG_DrawBulletImpacts1();
void mfh_CG_DrawBulletImpacts2();