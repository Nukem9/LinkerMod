#pragma once

#define MAX_WEAPONS 2048

static int *itemRegistered = (int *)0x01A775B8;

void ClearRegisteredItems();
void G_RegisterWeapon(unsigned int weapIndex);
int IsItemRegistered(int iItemIndex);