#include common_scripts\utility;
#include maps\_utility;
#include maps\_zombiemode_utility;
#include maps\_zombiemode_zone_manager; 

#include maps\zombie_theater_quad;

main()
{
	maps\template_fx::main();
	maps\template_amb::main();

	PreCacheModel("zombie_zapper_cagelight_red");
	precachemodel("zombie_zapper_cagelight_green");
	precacheShader("ac130_overlay_grain");	
	precacheshellshock( "electrocution" );
	// ww: viewmodel arms for the level
	PreCacheModel( "viewmodel_usa_pow_arms" ); // Dempsey
	PreCacheModel( "viewmodel_rus_prisoner_arms" ); // Nikolai
	PreCacheModel( "viewmodel_vtn_nva_standard_arms" );// Takeo
	PreCacheModel( "viewmodel_usa_hazmat_arms" );// Richtofen
	// DSM: models for light changing
	PreCacheModel("zombie_zapper_cagelight_on");
	precachemodel("zombie_zapper_cagelight");

	if(GetDvarInt( #"artist") > 0)
	{
		return;
	}
	
	level.dogs_enabled = true;	
	level.mixed_rounds_enabled = 1;//true;
	level.random_pandora_box_start = true;

	level.zombiemode_using_marathon_perk = true;
	level.zombiemode_using_divetonuke_perk = true;
	level.zombiemode_using_deadshot_perk = true;
	level.zombiemode_using_additionalprimaryweapon_perk = true; 
	
	level.zombie_anim_override = maps\template::anim_override_func;

	level thread maps\_callbacksetup::SetupCallbacks();
	
	level.quad_move_speed = 350;
	level.quad_traverse_death_fx = maps\zombie_theater_quad::quad_traverse_death_fx;
	level.quad_explode = true;

	level.dog_spawn_func = maps\_zombiemode_ai_dogs::dog_spawn_factory_logic;
	level.exit_level_func = ::template_exit_level;

	// Special zombie types, engineer and quads.
	level.custom_ai_type = [];
	level.custom_ai_type = array_add( level.custom_ai_type, maps\_zombiemode_ai_quad::init );
	level.custom_ai_type = array_add( level.custom_ai_type, maps\_zombiemode_ai_dogs::init );

	level.door_dialog_function = maps\_zombiemode::play_door_dialog;
	level.first_round_spawn_func = true;

	include_weapons();
	include_powerups();

	level.use_zombie_heroes = true;
	level.disable_protips = 1;

	// DO ACTUAL ZOMBIEMODE INIT
	maps\_zombiemode::main();
	// maps\_zombiemode_timer::init();

	// Turn off generic battlechatter - Steve G
	battlechatter_off("allies");
	battlechatter_off("axis");

	maps\_zombiemode_ai_dogs::enable_dog_rounds();

	init_template();
	
	// Setup the levels Zombie Zone Volumes
	maps\_compass::setupMiniMap("menu_map_template"); 
	level.ignore_spawner_func = ::template_ignore_spawner;

	level.zone_manager_init_func = ::template_zone_init;
	init_zones[0] = "test_zone";
	//init_zones[0] = "foyer_zone";
	//init_zones[1] = "foyer2_zone";	
	level thread maps\_zombiemode_zone_manager::manage_zones( init_zones );

	level thread maps\_zombiemode_auto_turret::init();

	init_sounds();
	level thread add_powerups_after_round_1();

	visionsetnaked( "template", 0 );

	//maps\zombie_theater_teleporter::teleport_pad_hide_use();

	//level.round_number = 1245;
}


#using_animtree( "generic_human" );
anim_override_func()
{
	level.scr_anim["zombie"]["walk7"] 	= %ai_zombie_walk_v8;	//goose step walk
}




//*****************************************************************************


theater_playanim( animname )
{
	self UseAnimTree(#animtree);
	self animscripted(animname + "_done", self.origin, self.angles, level.scr_anim[animname],"normal", undefined, 2.0  );
}


//*****************************************************************************
// WEAPON FUNCTIONS
//
// Include the weapons that are only in your level so that the cost/hints are accurate
// Also adds these weapons to the random treasure chest.
// Copy all include_weapon lines over to the level.csc file too - removing the weighting funcs...
//*****************************************************************************
include_weapons()
{
	include_weapon( "frag_grenade_zm", false, true );
	include_weapon( "claymore_zm", false, true );

	//	Weapons - Pistols
	include_weapon( "m1911_zm", false );						// colt
	include_weapon( "m1911_upgraded_zm", false );
	include_weapon( "python_zm" );						// 357
	include_weapon( "python_upgraded_zm", false );
  	include_weapon( "cz75_zm" );
  	include_weapon( "cz75_upgraded_zm", false );

	//	Weapons - Semi-Auto Rifles
	include_weapon( "m14_zm", false, true );							// gewehr43
	include_weapon( "m14_upgraded_zm", false );

	//	Weapons - Burst Rifles
	include_weapon( "m16_zm", false, true );						
	include_weapon( "m16_gl_upgraded_zm", false );
	include_weapon( "g11_lps_zm" );
	include_weapon( "g11_lps_upgraded_zm", false );
	include_weapon( "famas_zm" );
	include_weapon( "famas_upgraded_zm", false );

	//	Weapons - SMGs
	include_weapon( "ak74u_zm", false, true );						// thompson, mp40, bar
	include_weapon( "ak74u_upgraded_zm", false );
	include_weapon( "mp5k_zm", false, true );
	include_weapon( "mp5k_upgraded_zm", false );
	include_weapon( "mp40_zm", false, true );
	include_weapon( "mp40_upgraded_zm", false );
	include_weapon( "mpl_zm", false, true );
	include_weapon( "mpl_upgraded_zm", false );
	include_weapon( "pm63_zm", false, true );
	include_weapon( "pm63_upgraded_zm", false );
	include_weapon( "spectre_zm" );
	include_weapon( "spectre_upgraded_zm", false );

	//	Weapons - Dual Wield
  	include_weapon( "cz75dw_zm" );
  	include_weapon( "cz75dw_upgraded_zm", false );

	//	Weapons - Shotguns
	include_weapon( "ithaca_zm", false, true );						// shotgun
	include_weapon( "ithaca_upgraded_zm", false );
	include_weapon( "rottweil72_zm", false, true );
	include_weapon( "rottweil72_upgraded_zm", false );
	include_weapon( "spas_zm" );						// 
	include_weapon( "spas_upgraded_zm", false );
	include_weapon( "hs10_zm" );
	include_weapon( "hs10_upgraded_zm", false );

	//	Weapons - Assault Rifles
	include_weapon( "aug_acog_zm" );
	include_weapon( "aug_acog_mk_upgraded_zm", false );
	include_weapon( "galil_zm" );
	include_weapon( "galil_upgraded_zm", false );
	include_weapon( "commando_zm" );
	include_weapon( "commando_upgraded_zm", false );
	include_weapon( "fnfal_zm" );
	include_weapon( "fnfal_upgraded_zm", false );

	//	Weapons - Sniper Rifles
	include_weapon( "dragunov_zm" );					// ptrs41
	include_weapon( "dragunov_upgraded_zm", false );
	include_weapon( "l96a1_zm" );
	include_weapon( "l96a1_upgraded_zm", false );

	//	Weapons - Machineguns
	include_weapon( "rpk_zm" );							// mg42, 30 cal, ppsh
	include_weapon( "rpk_upgraded_zm", false );
	include_weapon( "hk21_zm" );
	include_weapon( "hk21_upgraded_zm", false );

	//	Weapons - Misc
	include_weapon( "m72_law_zm" );
	include_weapon( "m72_law_upgraded_zm", false );
	include_weapon( "china_lake_zm" );
	include_weapon( "china_lake_upgraded_zm", false );

	//	Weapons - Special
	include_weapon( "zombie_cymbal_monkey" );
	include_weapon( "ray_gun_zm" );
	include_weapon( "ray_gun_upgraded_zm", false );

	include_weapon( "thundergun_zm", true );
	include_weapon( "thundergun_upgraded_zm", false );
	include_weapon( "crossbow_explosive_zm" );
	include_weapon( "crossbow_explosive_upgraded_zm", false );

	include_weapon( "knife_ballistic_zm", true );
	include_weapon( "knife_ballistic_upgraded_zm", false );
	include_weapon( "knife_ballistic_bowie_zm", false );
	include_weapon( "knife_ballistic_bowie_upgraded_zm", false );
	level._uses_retrievable_ballisitic_knives = true;

	// limited weapons
	maps\_zombiemode_weapons::add_limited_weapon( "m1911_zm", 0 );
	maps\_zombiemode_weapons::add_limited_weapon( "thundergun_zm", 1 );
	maps\_zombiemode_weapons::add_limited_weapon( "crossbow_explosive_zm", 1 );
	maps\_zombiemode_weapons::add_limited_weapon( "knife_ballistic_zm", 1 );

	precacheItem( "explosive_bolt_zm" );
	precacheItem( "explosive_bolt_upgraded_zm" );
}

//*****************************************************************************
// POWERUP FUNCTIONS
//*****************************************************************************

include_powerups()
{
	include_powerup( "nuke" );
	include_powerup( "insta_kill" );
	include_powerup( "double_points" );
	include_powerup( "full_ammo" );
	include_powerup( "carpenter" );
	include_powerup( "fire_sale" );
}

add_powerups_after_round_1()
{
	
	//want to precache all the stuff for these powerups, but we don't want them to be available in the first round
	level.zombie_powerup_array = array_remove (level.zombie_powerup_array, "nuke"); 
	level.zombie_powerup_array = array_remove (level.zombie_powerup_array, "fire_sale");

	while (1)
	{
		if (level.round_number > 1)
		{
			level.zombie_powerup_array = array_add(level.zombie_powerup_array, "nuke");
			level.zombie_powerup_array = array_add(level.zombie_powerup_array, "fire_sale");
			break;
		}
		wait (1);
	}
}			

//*****************************************************************************

init_template()
{
	flag_init( "curtains_done" );
	flag_init( "lobby_occupied" );
	flag_init( "dining_occupied" );
	flag_init( "special_quad_round" );


	level thread electric_switch();		

	level thread teleporter_intro();
}

//*****************************************************************************
teleporter_intro()
{
	flag_wait( "all_players_spawned" );

	wait( 0.25 );

	players = get_players();
	for ( i = 0; i < players.size; i++ )
	{
		players[i] SetTransported( 2 );
	}
	
	playsoundatposition( "evt_beam_fx_2d", (0,0,0) );
    playsoundatposition( "evt_pad_cooldown_2d", (0,0,0) );
}

//*****************************************************************************
// ELECTRIC SWITCH
// once this is used, it activates other objects in the map
// and makes them available to use
//*****************************************************************************
electric_switch()
{
	trig = getent("use_elec_switch","targetname");
	trig sethintstring(&"ZOMBIE_ELECTRIC_SWITCH");
	trig setcursorhint( "HINT_NOICON" );


	level thread wait_for_power();

	trig waittill("trigger",user);

	trig delete();	
	flag_set( "power_on" );
	Objective_State(8,"done");

	//Enable quad zombie spawners
	reinit_zone_spawners();
}


//
//	Wait for the power_on flag to be set.  This is needed to work in conjunction with
//		the devgui cheat.
//
wait_for_power()
{
	master_switch = getent("elec_switch","targetname");	
	master_switch notsolid();

	flag_wait( "power_on" );

	master_switch rotateroll(-90,.3);
	master_switch playsound("zmb_switch_flip");

	clientnotify( "ZPO" );		// Zombie power on.

	master_switch waittill("rotatedone");
	playfx(level._effect["switch_sparks"] ,getstruct("elec_switch_fx","targetname").origin);
	
	//Sound - Shawn J  - adding temp sound to looping sparks & turning on power sources
	master_switch playsound("zmb_turn_on");

	//get the teleporter ready
	//maps\zombie_theater_teleporter::teleporter_init();	
	wait_network_frame();
	// Set Perk Machine Notifys
	level notify("revive_on");
	wait_network_frame();
	level notify("juggernog_on");
	wait_network_frame();
	level notify("sleight_on");
	wait_network_frame();
	level notify("doubletap_on");
	wait_network_frame();
	level notify("Pack_A_Punch_on" );	
	wait_network_frame();

	//SE2Dev - Enable additional perks
	level notify("marathon_on" );	
	wait_network_frame();
	level notify("divetonuke_on" );	
	wait_network_frame();
	level notify("deadshot_on" );	
	wait_network_frame();
	level notify("additionalprimaryweapon_on" );	
	wait_network_frame();

	// start quad round
	// Set number of quads per round
	players = get_players();
	level.quads_per_round = 4 * players.size;	// initial setting

	level notify("quad_round_can_end");
	level.delay_spawners = undefined;
	
	// DCS: start check for potential quad waves after power turns on.
	level thread quad_wave_init();
}

//AUDIO

init_sounds()
{
	maps\_zombiemode_utility::add_sound( "wooden_door", "zmb_door_wood_open" );
	maps\_zombiemode_utility::add_sound( "fence_door", "zmb_door_fence_open" );
}


// *****************************************************************************
// Zone management
// *****************************************************************************

template_zone_init()
{
	flag_init( "always_on" );
	flag_set( "always_on" );

	// foyer_zone
	add_adjacent_zone( "test_zone", "test_zone", "always_on" );	

	// theater_zone
	//add_adjacent_zone( "theater_zone", "foyer2_zone", "power_on" );
	//add_adjacent_zone( "theater_zone", "stage_zone", "power_on" );
}	

template_ignore_spawner( spawner )
{
	// no power, no quads
	if ( !flag("power_on") )
	{
		if ( spawner.script_noteworthy == "quad_zombie_spawner" )
		{
			return true;
		}
	}

	return false;
}

// *****************************************************************************
// 	DCS: random round change quad emphasis
// 	This should only happen in zones where quads spawn into
// 	and crawl down the wall.
//	potential zones: foyer_zone, theater_zone, stage_zone, dining_zone
// *****************************************************************************
quad_wave_init()
{
	level thread time_for_quad_wave("foyer_zone");
	level thread time_for_quad_wave("theater_zone");
	level thread time_for_quad_wave("stage_zone");
	level thread time_for_quad_wave("dining_zone");
	
	level waittill( "end_of_round" );
	flag_clear( "special_quad_round" );	
}

time_for_quad_wave(zone_name)
{

	if(!IsDefined(zone_name))
	{
		return;
	}
	zone = level.zones[ zone_name ];

	//	wait for round change.
	level waittill( "between_round_over" );
	
	//avoid dog rounds.
	if ( IsDefined( level.next_dog_round ) && level.next_dog_round == level.round_number )
	{
		level thread time_for_quad_wave(zone_name);			
		return;
	}	

	// ripped from spawn script for accuracy.	-------------------------------------
	max = level.zombie_vars["zombie_max_ai"];
	multiplier = level.round_number / 5;
	if( multiplier < 1 )
	{
		multiplier = 1;
	}

	if( level.round_number >= 10 )
	{
		multiplier *= level.round_number * 0.15;
	}

	player_num = get_players().size;

	if( player_num == 1 )
	{
		max += int( ( 0.5 * level.zombie_vars["zombie_ai_per_player"] ) * multiplier ); 
	}
	else
	{
		max += int( ( ( player_num - 1 ) * level.zombie_vars["zombie_ai_per_player"] ) * multiplier ); 
	}
	// ripped from spawn script for accuracy.	-------------------------------------

	//percent chance.
	chance = 100;
	max_zombies = [[ level.max_zombie_func ]]( max );
	current_round = level.round_number;
	
	// every third round a chance of a quad wave.
	if((level.round_number % 3 == 0) && chance >= RandomInt(100))
	{	
		if(zone.is_occupied)
		{
			flag_set( "special_quad_round" );
			maps\_zombiemode_zone_manager::reinit_zone_spawners();

			while( level.zombie_total < max_zombies /2 && current_round == level.round_number )
			{
				wait(0.1);
			}

			//level waittill( "end_of_round" );

			flag_clear( "special_quad_round" );
			maps\_zombiemode_zone_manager::reinit_zone_spawners();
			
		}
	}
	level thread time_for_quad_wave(zone_name);
}

template_exit_level()
{
	zombies = GetAiArray( "axis" );
	for ( i = 0; i < zombies.size; i++ )
	{
		zombies[i] thread template_find_exit_point();
	}
}

template_find_exit_point()
{
	self endon( "death" );

	player = getplayers()[0];

	dist_zombie = 0;
	dist_player = 0;
	dest = 0;

	away = VectorNormalize( self.origin - player.origin );
	endPos = self.origin + vector_scale( away, 600 );

	locs = array_randomize( level.enemy_dog_locations );

	for ( i = 0; i < locs.size; i++ )
	{
		dist_zombie = DistanceSquared( locs[i].origin, endPos );
		dist_player = DistanceSquared( locs[i].origin, player.origin );

		if ( dist_zombie < dist_player )
		{
			dest = i;
			break;
		}
	}

	self notify( "stop_find_flesh" );
	self notify( "zombie_acquire_enemy" );

	self setgoalpos( locs[dest].origin );

	while ( 1 )
	{
		if ( !flag( "wait_and_revive" ) )
		{
			break;
		}
		wait_network_frame();
	}
	
	self thread maps\_zombiemode_spawner::find_flesh();
} 