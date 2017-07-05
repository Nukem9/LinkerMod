#pragma once

struct entity_s;
struct brush_s
{
	char pad[4];
	brush_s *next;
	entity_s *owner;
	brush_s *ownerNext;
};
STATIC_ASSERT_OFFSET(brush_s, next, 0x4);
STATIC_ASSERT_OFFSET(brush_s, owner, 0x8);
STATIC_ASSERT_OFFSET(brush_s, ownerNext, 0xC);

static brush_s *selected_brushes = (brush_s *)0x0256459C;