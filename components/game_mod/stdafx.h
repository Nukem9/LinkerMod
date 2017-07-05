#pragma once

#pragma comment(lib, "detours.lib")
#include "../shared/detours/Detours.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <intrin.h>
#include <Psapi.h>
#include <shellapi.h>
#include <winsock.h>
#include <time.h>
#include <vector>
#include <unordered_map>

#pragma comment(lib, "ws2_32.lib")

//
// Shared files
//
#include "../shared/shared_utility.h"
#include "../shared/shared_version.h"

#include "../shared/minidx9/Include/d3dx9.h"
#pragma comment(lib, "../shared/minidx9/Lib/x86/d3dx9.lib")

#include "CEG.h"

#include "bitarray.h"
#include "d3d9ex.h"
#include "common.h"
#include "com_files.h"
#include "com_math.h"
#include "com_memory.h"
#include "cmd.h"
#include "dvar.h"
#include "g_main_mp.h"
#include "bg_weapons_def.h"
#include "bg_weapons_ammo.h"
#include "bg_weapons_load_obj.h"
#include "g_items.h"
#include "sv_ccmds_mp.h"
#include "sv_client_mp.h"
#include "cl_main_mp.h"
#include "cl_console.h"
#include "db_registry.h"
#include "ui_main_pc.h"
#include "live_win.h"
#include "live_steam_client.h"
#include "dwlogon_pc.h"
#include "cg_scoreboard.h"
#include "cg_draw_debug.h"
#include "sys_cmds.h"
#include "r_init.h"
#include "r_image.h"
#include "r_debug.h"
#include "r_rendercmds.h"
#include "r_cinematic.h"
#include "r_screenshot.h"
#include "r_scene.h"
#include "r_reflection_probe.h"
#include "r_material.h"
#include "r_material_load_obj.h"
#include "r_draw_xmodel.h"
#include "rb_tess.h"
#include "cscr_vm.h"
#include "scr_cmds.h"
#include "cscr_parser.h"
#include "threads.h"
#include "ragdoll.h"
#include "ragdoll_update.h"
#include "phys_main.h"
#include "com_bsp_load_obj.h"
#include "bullet.h"
#include "cg_weapons.h"
#include "bg_perks.h"
#include "cl_debugdata.h"
#include "bg_weapons.h"
#include "cl_keys.h"
#include "sv_main_mp.h"
#include "pathnode.h"
#include "pathnode_load_obj.h"
#include "radiant_remote.h"
#include "g_client_script_cmd_mp.h"

#include "win_localize.h"
#include "win_exception.h"
#include "win_common.h"
#include "win_main.h"

#include "patch_common.h"
#include "patch_reflections.h"
#include "patch_usefastfile.h"

#include "reshade.h"

#include "mod_frontend.h"

#define GM_NET_VERSION 0x01

#define _USE_LEVEL_DEPENDENCIES 1
