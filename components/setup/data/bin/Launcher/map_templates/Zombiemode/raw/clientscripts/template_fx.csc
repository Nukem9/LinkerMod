#include clientscripts\_utility; 
#include clientscripts\_fx;
#include clientscripts\_music;
main()
{
	clientscripts\createfx\template_fx::main();
	clientscripts\_fx::reportNumEffects();
	precache_util_fx();
	precache_createfx_fx();
	disableFX = GetDvarInt( #"disable_fx" );
	if( !IsDefined( disableFX ) || disableFX <= 0 )
	{
		precache_scripted_fx();
	}
	
	level thread clientscripts\_zombiemode::init_perk_machines_fx();
	level thread dog_start_monitor();
	level thread dog_stop_monitor();
	level notify( "dog_stop" ); //DCS: will reset on restart.
}

precache_util_fx()
{
}

precache_scripted_fx()
{
	level._effect["eye_glow"]			 										= LoadFx( "misc/fx_zombie_eye_single" ); 
	level._effect["headshot"] 					= LoadFX( "impacts/fx_flesh_hit" );
	level._effect["headshot_nochunks"] 			= LoadFX( "misc/fx_zombie_bloodsplat" );
	level._effect["bloodspurt"] 				= LoadFX( "misc/fx_zombie_bloodspurt" );
	level._effect["animscript_gib_fx"] 		 = LoadFx( "weapon/bullet/fx_flesh_gib_fatal_01" ); 
	level._effect["animscript_gibtrail_fx"] 	 = LoadFx( "trail/fx_trail_blood_streak" ); 	
	
}

precache_createfx_fx()
{
	//SE2Dev Effects
	//level._effect["snow_flakes_windy_blizzard"] = loadfx("env/weather/fx_snow_blizzard_intense2");
	//level._effect["axis_fx_test"] = loadFX("fx_tools/fx_tools_axis_xlg");
	level._effect["fx_mp_smoke_thick_indoor"]									= loadfx("maps/zombie/fx_mp_smoke_thick_indoor");
	level._effect["fx_smoke_smolder_md_gry"]									= loadfx("maps/zombie/fx_smoke_smolder_md_gry");
	level._effect["fx_smk_smolder_sm"]												= loadfx("env/smoke/fx_smk_smolder_sm");
	level._effect["fx_mp_smoke_crater"]					      				= loadfx("maps/zombie/fx_mp_smoke_crater");
	level._effect["fx_mp_smoke_sm_slow"]											= loadfx("maps/zombie/fx_mp_smoke_sm_slow");	
	
	level._effect["fx_mp_fog_low"]														= loadfx("maps/zombie/fx_mp_fog_low");
	level._effect["fx_mp_fog_ground_md"]											= loadfx("maps/mp_maps/fx_mp_fog_ground_md");	
	
	level._effect["fx_water_drip_light_long"]									= loadfx("env/water/fx_water_drip_light_long");	
	level._effect["fx_water_drip_light_short"]								= loadfx("env/water/fx_water_drip_light_short");
	
	level._effect["fx_mp_ray_light_sm"]												= loadfx("env/light/fx_light_godray_overcast_sm");
	level._effect["fx_mp_ray_light_md"]												= loadfx("maps/zombie/fx_mp_ray_overcast_md");
	level._effect["fx_mp_ray_light_lg"]												= loadfx("maps/zombie/fx_light_godray_overcast_lg");
	
	level._effect["fx_mp_dust_motes"]													= loadfx("maps/zombie/fx_mp_ray_motes_lg");
	level._effect["fx_mp_dust_mote_pcloud_sm"]								= loadfx("maps/zombie/fx_mp_dust_mote_pcloud_sm");	
	level._effect["fx_mp_dust_mote_pcloud_md"]								= loadfx("maps/zombie/fx_mp_dust_mote_pcloud_md");	
	
	level._effect["fx_mp_pipe_steam"]													= loadfx("env/smoke/fx_pipe_steam_md");
	level._effect["fx_mp_pipe_steam_random"]									= loadfx("maps/zombie/fx_mp_pipe_steam_random");
	level._effect["fx_mp_fumes_vent_sm_int"]									= loadfx("maps/mp_maps/fx_mp_fumes_vent_sm_int");
	level._effect["fx_mp_fumes_vent_xsm_int"]									= loadfx("maps/mp_maps/fx_mp_fumes_vent_xsm_int");
	
	level._effect["fx_mp_elec_spark_burst_xsm_thin_runner"]		= loadfx("maps/mp_maps/fx_mp_elec_spark_burst_xsm_thin_runner");
	level._effect["fx_mp_elec_spark_burst_sm_runner"]					= loadfx("maps/mp_maps/fx_mp_elec_spark_burst_sm_runner");
	
	level._effect["fx_mp_light_lamp"]													= loadfx("maps/zombie/fx_mp_light_lamp");
	level._effect["fx_mp_light_corona_cool"]									= loadfx("maps/zombie/fx_mp_light_corona_cool");
	level._effect["fx_mp_light_corona_bulb_ceiling"]					= loadfx("maps/zombie/fx_mp_light_corona_bulb_ceiling");
	level._effect["fx_pent_tinhat_light"]                			= LoadFX("maps/pentagon/fx_pent_tinhat_light");	
	level._effect["fx_light_floodlight_bright"]								= loadfx("maps/zombie/fx_zombie_light_floodlight_bright");		
	level._effect["fx_light_overhead_sm_amber"]								= loadfx("maps/zombie/fx_zombie_overhead_sm_amber");
	level._effect["fx_light_overhead_sm_amber_flkr"]					= loadfx("maps/zombie/fx_zombie_overhead_sm_amber_flkr");
	level._effect["fx_light_overhead_sm_blue"]								= loadfx("maps/zombie/fx_zombie_overhead_sm_blu");
	level._effect["fx_light_overhead_sm_blue_flkr"]						= loadfx("maps/zombie/fx_zombie_overhead_sm_blu_flkr");
	
	level._effect["fx_mp_birds_circling"]											= loadfx("maps/zombie/fx_mp_birds_circling");	
	level._effect["fx_mp_insects_lantern"]										= loadfx("maps/zombie/fx_mp_insects_lantern");
	level._effect["fx_insects_swarm_md_light"]								= loadfx("bio/insects/fx_insects_swarm_md_light");
	level._effect["fx_insects_maggots"]												= loadfx("bio/insects/fx_insects_maggots_sm");
	level._effect["fx_insects_moths_light_source"]						= loadfx("bio/insects/fx_insects_moths_light_source");
	level._effect["fx_insects_moths_light_source_md"]					= loadfx("bio/insects/fx_insects_moths_light_source_md");
	
	// Teleporter FX
	level._effect["fx_transporter_beam"]									= loadfx("maps/zombie/fx_transporter_beam");
	level._effect["fx_transporter_pad_start"]							= loadfx("maps/zombie/fx_transporter_pad_start");
	level._effect["fx_transporter_start"]									= loadfx("maps/zombie/fx_transporter_start");		
	level._effect["fx_transporter_ambient"]				    		= loadfx("maps/zombie/fx_transporter_ambient");		
	level._effect["fx_zombie_mainframe_beam"]							= loadfx("maps/zombie/fx_zombie_mainframe_beam");	
	level._effect["fx_zombie_mainframe_flat"]							= loadfx("maps/zombie/fx_zombie_mainframe_flat");		
	level._effect["fx_zombie_mainframe_flat_start"]				= loadfx("maps/zombie/fx_zombie_mainframe_flat_start");		
	level._effect["fx_zombie_mainframe_beam_start"]				= loadfx("maps/zombie/fx_zombie_mainframe_beam_start");		
	level._effect["fx_zombie_flashback_theater"]					= loadfx("maps/zombie/fx_zombie_flashback_theater");		
	level._effect["fx_zombie_difference"]									= loadfx("maps/zombie/fx_zombie_difference");
	level._effect["fx_zombie_heat_sink"]									= loadfx("maps/zombie/fx_zombie_heat_sink");
	//PI ESM - trap fx
	level._effect["zapper"]					= loadfx("misc/fx_zombie_electric_trap");
	level._effect["switch_sparks"]	= loadfx("env/electrical/fx_elec_wire_spark_burst");
	
	// PI ESM - for fire trap
	level._effect["fire_trap_med"] 	= loadfx("maps/zombie/fx_zombie_fire_trap_med");	
	
	// Quad fx
	level._effect["fx_quad_roof_break"]							= loadfx("maps/zombie/fx_zombie_crawler_roof_break");	
	level._effect["fx_quad_roof_break_theater"]			= loadfx("maps/zombie/fx_zombie_crawler_roof_theater");	
	level._effect["fx_quad_dust_roof"]							= loadfx("maps/zombie/fx_zombie_crawler_dust_roof");	
}

//*****************************************************************************
// TRAPS:
//		name = targetname of the fx structs
//		loc	= abbreviated notify (try to keep to 2 characters max)
//*****************************************************************************
trap_fx_monitor( name, loc, trap_type )
{
	// Get the list of points.
	//	Don't include structs used for script_models
	structs = getstructarray( name, "targetname" );
	points = [];
	for ( i=0; i<structs.size; i++ )
	{
		if ( !IsDefined(structs[i].model ) )
		{
			points[ points.size ] = structs[i];
		}
	}
	
	while(1)
	{
		// Wait for the notify
		level waittill( loc+"1" );	// "one" means on
		// Spawn an FX for each struct
		for( i=0; i<points.size; i++ )
		{
			points[i] thread trap_play_fx( loc, trap_type );
		}
	}
}

//*****************************************************************************
// TRAP FX
//*****************************************************************************
trap_play_fx( loc, trap_type )
{
	// Get the structs orientation
	ang = self.angles;
	forward = anglestoforward( ang );
	up = anglestoup( ang );
	
	// If the effects already exist, delete them
	if ( isdefined( self.loopFX ) )
	{
		for( i=0; i<self.loopFX.size; i++ )
		{
			self.loopFX[i] delete();
		}
		
		self.loopFX = [];
	}
	if(!isdefined(self.loopFX))
	{
		self.loopFX = [];
	}	
	// Effect override
	fx_name = "";	// scope declaration
	if ( IsDefined( self.script_string ) )
	{
		fx_name = self.script_string;
	}
	else
	{
		switch( trap_type )
		{
		case "electric":
			fx_name = "zapper";
			break;
		default:
		case "fire":
			fx_name = "fire_trap_med";
			break;
		}
	}
	// Create the Effect for each player
	players = getlocalplayers();
	for( i=0; i<players.size; i++ )
	{
		self.loopFX[i] = SpawnFx( i, level._effect[ fx_name ], self.origin, 0, forward, up );
		triggerfx( self.loopFX[i] );
	}
	
	// Wait for the server notify to stop the effect
	level waittill( loc + "0" );	// "zero" means off
	
	// Delete the effects
	for(i=0; i<self.loopFX.size; i++ )
	{
		self.loopFX[i] delete();
	}
	self.loopFX = [];
	
}
//  Replace the light models when the lights turn on and off
light_model_swap( name, model )
{
		level waittill( "pl1" );	// Power lights on
		players = getlocalplayers();
		for ( p=0; p<players.size; p++ )
		{
			lamps = GetEntArray( p, name, "targetname" );
			for ( i=0; i<lamps.size; i++ )
			{
				lamps[i] SetModel( model );
			}
		}
}          

//
// Pulls the fog in
dog_start_monitor()
{
	while( 1 )
	{
		level waittill( "dog_start" );
			start_dist = 0;
		half_dist = 302.622;
		half_height = 306.395;
		base_height = 344.622;
	fog_r = 0.458824;
	fog_g = 0.556863;
	fog_b = 0.619608;
	fog_scale = 1;
	sun_col_r = 0.243137;
	sun_col_g = 0.270588;
	sun_col_b = 0.270588;
	sun_dir_x = 0.291692;
	sun_dir_y = -0.720765;
	sun_dir_z = 0.628819;
	sun_start_ang = 0;
	sun_stop_ang = 0;
		time = 7;
		max_fog_opacity = 0.65;
	setVolFog(start_dist, half_dist, half_height, base_height, fog_r, fog_g, fog_b, fog_scale,
		sun_col_r, sun_col_g, sun_col_b, sun_dir_x, sun_dir_y, sun_dir_z, sun_start_ang, 
		sun_stop_ang, time, max_fog_opacity);
	}
}

//
// Pushes fog out
dog_stop_monitor()
{
	while( 1 )
	{
		level waittill( "dog_stop" );

start_dist = 155.625;
	half_dist = 2332.79;
	half_height = 677.241;
	base_height = 23.2733;
	fog_r = 0.639216;
	fog_g = 0.639216;
	fog_b = 0.639216;
	fog_scale = 1;
	sun_col_r = 0.243137;
	sun_col_g = 0.270588;
	sun_col_b = 0.270588;
	sun_dir_x = 0.291692;
	sun_dir_y = -0.720765;
	sun_dir_z = 0.628819;
	sun_start_ang = 0;
	sun_stop_ang = 0;
	time = 7;
	max_fog_opacity = 1;
	
	setVolFog(start_dist, half_dist, half_height, base_height, fog_r, fog_g, fog_b, fog_scale,
		sun_col_r, sun_col_g, sun_col_b, sun_dir_x, sun_dir_y, sun_dir_z, sun_start_ang, 
		sun_stop_ang, time, max_fog_opacity);
	}
}
