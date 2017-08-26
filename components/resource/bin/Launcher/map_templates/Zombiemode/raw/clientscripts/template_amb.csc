//
// file: template_amb.csc
// description: clientside ambient script for template: setup ambient sounds, etc.
//

#include clientscripts\_utility; 
#include clientscripts\_ambientpackage;
#include clientscripts\_music;
#include clientscripts\_busing;
#include clientscripts\_audio;

main()
{
 
    //**** AMBIENT PACKAGES/ROOMS ****\\
    
/* 
 	declareAmbientRoom( "theater_main_room" );
 	declareAmbientPackage( "theater_main_room" );
 	setAmbientRoomTone( "theater_main_room", "amb_theater_bg" );
 	setAmbientRoomReverb ("theater_main_room","zmb_theater_main_room", 1, 1);
        addAmbientElement( "theater_main_room", "amb_wood_groan", 15, 60, 50, 150 );
        setAmbientRoomContext( "theater_main_room", "ringoff_plr", "indoor" );                                                                  

	//Default Ambient Package
        activateAmbientPackage( 0, "theater_main_room", 0 );
        activateAmbientRoom( 0, "theater_main_room", 0 );
*/

//MUSIC STATES
/*		
	declareMusicState("WAVE");
		musicAliasloop("mus_theatre_underscore", 4, 2);	
		
	declareMusicState("EGG");
		musicAlias("mus_egg", 1);
		
	declareMusicState( "SILENCE" );
	    musicAlias("null", 1 );	
*/		
	//THREADED FUNCTIONS
	thread power_on_all();	
}

//POWER ON
power_on_all()
{
	level waittill( "pl1" );

	//Post power ambience
}
