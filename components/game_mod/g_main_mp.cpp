#include "stdafx.h"

// /game_mp/g_main_mp.cpp:1497
clientState_s *G_GetClientState(int clientNum)
{
	//if (!Demo_IsPlaying())
	//	ASSERT(clientNum >= 0 && clientNum < level.maxclients, "clientNum doesn't index level.maxclients");

	return &level.clients[clientNum].sess.cs;
}

// /game_mp/g_main_mp.cpp:1514
playerState_s *G_GetPlayerState(int clientNum)
{
	//if (!Demo_IsPlaying())
	//	ASSERT(clientNum >= 0 && clientNum < level.maxclients, "clientNum doesn't index level.maxclients");

	return &level.clients[clientNum].ps;
}