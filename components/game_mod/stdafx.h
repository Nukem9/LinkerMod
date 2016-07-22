#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <intrin.h>
#include <Psapi.h>

//
// Shared files
//
#include "../shared/utility.h"
//
// 
//

#include "CEG.h"

#include "common.h"
#include "com_files.h"
#include "cmd.h"
#include "dvar.h"
#include "sv_ccmds_mp.h"
#include "cl_main_mp.h"
#include "cl_console.h"
#include "db_registry.h"
#include "ui_main_pc.h"
#include "live_win.h"

#include "cg_scoreboard.h"

#include "sys_cmds.h"
#include "r_rendercmds.h"
#include "r_cinematic.h"
#include "r_screenshot.h"
#include "r_scene.h"
#include "r_reflection_probe.h"
#include "r_material_load_obj.h"

#include "win_localize.h"
#include "win_exception.h"
#include "win_common.h"

#include "patch_reflections.h"

#include "reshade.h"

#define GM_NET_VERSION 0x01
