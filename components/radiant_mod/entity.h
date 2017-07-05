#pragma once

struct eclass_s
{
	char _pa1[0x4];
	const char *name;
};
STATIC_ASSERT_OFFSET(eclass_s, name, 0x4);

struct epair_s
{
	epair_s *next;
	char	*key;
	char	*value;
};
STATIC_ASSERT_OFFSET(epair_s, next, 0x0);
STATIC_ASSERT_OFFSET(epair_s, key, 0x4);
STATIC_ASSERT_OFFSET(epair_s, value, 0x8);

struct entity_s
{
	entity_s *prev;
	entity_s *next;
	char _pad1[0x58]; // brushes @ 0xC
	eclass_s *eclass;
	char _pad2[0x4];
	float origin[3];
	epair_s *epairs;
	char _pad3[0x10];
	int refCount;
};
STATIC_ASSERT_OFFSET(entity_s, prev, 0x0);
STATIC_ASSERT_OFFSET(entity_s, next, 0x4);
//STATIC_ASSERT_OFFSET(entity_s, brushes, 0xC);
STATIC_ASSERT_OFFSET(entity_s, eclass, 0x60);
STATIC_ASSERT_OFFSET(entity_s, origin, 0x68);
STATIC_ASSERT_OFFSET(entity_s, epairs, 0x74);
STATIC_ASSERT_OFFSET(entity_s, refCount, 0x88);

static int& d_select_count = *(int *)0x02857C5C;	// Total number of selected objects
static int& multiple_entities = *(int *)0x0257CEDC;	// True if more than one entity is selected

static entity_s*& g_qeglobals_d_project_entity = *(entity_s **)0x027E1824;
static entity_s* world_entity = (entity_s *)0x027C3400;
static entity_s* edit_entity = (entity_s *)0x0257CED8;

extern void(*Entity_Clone_o)();
extern void(*Entity_Free_o)(entity_s *ent);

extern void(*SetEntityKeyValue_o)(entity_s *ent, const char *key, const char *value);
extern void(*SetKeyValue_o)(epair_s *pair, const char *key, const char *value);

void hk_Entity_Free(entity_s *ent);
void hk_Entity_Clone();
void hk_SetEntityKeyValue(entity_s *ent, const char *key, const char *value);
void hk_SetKeyValue(const char *key, const char *value);