#include maps\_utility; 
#include common_scripts\utility;
main()
{
	scriptedFX();
	footsteps(); 	
	//maps\createfx\template_fx::main();
	precacheFX();
}
footsteps()
{
}
scriptedFX()
{
	level._effect["animscript_gibtrail_fx"] 	 = LoadFx( "trail/fx_trail_blood_streak" ); 	//crashes the game	
	level._effect["animscript_gib_fx"] 		 = LoadFx( "weapon/bullet/fx_flesh_gib_fatal_01" ); 
	level._effect["animscript_gibtrail_fx"] 	 = LoadFx( "trail/fx_trail_blood_streak" ); 	
	
	level._effect["large_ceiling_dust"]			= loadfx( "maps/zombie/fx_dust_ceiling_impact_lg_mdbrown" );
	level._effect["poltergeist"]						= loadfx( "misc/fx_zombie_couch_effect" );
	
	// electric switch fx
	level._effect["switch_sparks"]					= loadfx("env/electrical/fx_elec_wire_spark_burst");
	
	// dogs
	level._effect["dog_breath"] 						= loadfx("maps/zombie/fx_zombie_dog_breath");	
	
	// rise fx 
	level._effect["rise_burst"]							= loadfx("maps/mp_maps/fx_mp_zombie_hand_dirt_burst");
	level._effect["rise_billow"]						= loadfx("maps/mp_maps/fx_mp_zombie_body_dirt_billowing");
	level._effect["rise_dust"]							= loadfx("maps/mp_maps/fx_mp_zombie_body_dust_falling");	
	
	// quad fx
	level._effect["quad_grnd_dust_spwnr"]		= loadfx("maps/zombie/fx_zombie_crawler_grnd_dust_spwnr");	
	level._effect["quad_grnd_dust"]					= loadfx("maps/zombie/fx_zombie_crawler_grnd_dust");	
	
	level._effect["lght_marker"] 								= Loadfx("maps/zombie/fx_zombie_coast_marker");
	level._effect["lght_marker_flare"] 					= Loadfx("maps/zombie/fx_zombie_coast_marker_fl");	
	
	//electrical trap
	level._effect["electric_current"] 					= loadfx("misc/fx_zombie_elec_trail");
	level._effect["zapper_fx"] 									= loadfx("misc/fx_zombie_zapper_powerbox_on");	
	level._effect["zapper"]											= loadfx("misc/fx_zombie_electric_trap");
	level._effect["zapper_wall"] 								= loadfx("misc/fx_zombie_zapper_wall_control_on");
	level._effect["zapper_light_ready"] 				= loadfx("misc/fx_zombie_zapper_light_green");
	level._effect["zapper_light_notready"] 			= loadfx("misc/fx_zombie_zapper_light_red");	
	level._effect["elec_md"] 		= loadfx("maps/zombie/fx_elec_player_md");
	level._effect["elec_sm"] 		= loadfx("maps/zombie/fx_elec_player_sm");	
	level._effect["elec_torso"] = loadfx("maps/zombie/fx_elec_player_torso");
	// fire trap
	level._effect["fire_trap_med"] = loadfx("maps/zombie/fx_zombie_fire_trap_med");	
	// auto turret
	level._effect["auto_turret_light"] = loadfx("maps/zombie/fx_zombie_auto_turret_light");
}
	
precachefx()
{
	level._effect["fx_mp_smoke_thick_indoor"]									= loadfx("maps/zombie/fx_mp_smoke_thick_indoor");
	level._effect["fx_mp_smoke_amb_indoor_misty"]							= loadfx("maps/zombie/fx_zombie_theater_smoke_amb_indoor");
	level._effect["fx_smoke_smolder_md_gry"]									= loadfx("maps/zombie/fx_smoke_smolder_md_gry");
	level._effect["fx_smk_smolder_sm"]												= loadfx("env/smoke/fx_smk_smolder_sm");
	level._effect["fx_mp_smoke_crater"]					      				= loadfx("maps/zombie/fx_mp_smoke_crater");
	level._effect["fx_mp_smoke_sm_slow"]											= loadfx("maps/zombie/fx_mp_smoke_sm_slow");	
	
	level._effect["fx_mp_fog_low"]														= loadfx("maps/zombie/fx_mp_fog_low");
	level._effect["fx_zombie_theater_fog_lg"]									= loadfx("maps/zombie/fx_zombie_theater_fog_lg");
	level._effect["fx_zombie_theater_fog_xlg"]								= loadfx("maps/zombie/fx_zombie_theater_fog_xlg");
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
	level._effect["fx_teleporter_pad_glow"]										= loadfx("maps/zombie/fx_zombie_teleporter_pad_glow");
	level._effect["fx_portal"]																= loadfx("maps/zombie/fx_zombie_portal_nix_num_pp");
	
	// Pack A Punch
	level._effect["zombie_packapunch"]										= loadfx("maps/zombie/fx_zombie_packapunch");
	
	// added for magic box lights
	level._effect["boxlight_light_ready"] 								= loadfx("maps/zombie/fx_zombie_theater_lightboard_green");
	level._effect["boxlight_light_notready"] 							= loadfx("maps/zombie/fx_zombie_theater_lightboard_red");
    
	// Quad fx
	level._effect["fx_quad_roof_break"]										= loadfx("maps/zombie/fx_zombie_crawler_roof_break");	
	level._effect["fx_quad_roof_break_theater"]						= loadfx("maps/zombie/fx_zombie_crawler_roof_theater");	
	level._effect["fx_quad_dust_roof"]										= loadfx("maps/zombie/fx_zombie_crawler_dust_roof");	
}	
