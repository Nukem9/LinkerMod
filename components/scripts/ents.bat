:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: Automatically generate Radiant compatible map files from the ent strings of all game maps ::
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

@echo off

set GAME_DIR=..\..
set ASSET_UTIL=%GAME_DIR%\bin\asset_util.exe

set FF_DIR=%GAME_DIR%\zone\Common\

set SP_MAPS=cuba vorkuta pentagon flashpoint khe_sanh hue_city kowloon fullahead creek_1 river wmd_sr71 wmd pow rebirth int_escape underwaterbase
set ZM_MAPS=zombie_theater zombie_pentagon zombietron zombie_cosmodrome zombie_coast zombie_temple zombie_moon zombie_cod5_asylum zombie_cod5_factory zombie_cod5_prototype zombie_cod5_sumpf
set MP_MAPS=mp_area51 mp_array mp_berlinwall2 mp_cairo mp_cosmodrome mp_cracked mp_crisis mp_discovery mp_drivein mp_duga mp_firingrange mp_golfcourse mp_gridlock mp_hanoi mp_havoc mp_hotel mp_kowloon mp_mountain mp_nuked mp_outskirts mp_radiation mp_russianbase mp_silo mp_stadium mp_villa mp_zoo

set MAP=

set OUT_DIR=%GAME_DIR%\map_source\_prefabs\maps\
if not exist %OUT_DIR% mkdir %OUT_DIR%

(for %%m in (%SP_MAPS%) do (
	set MAP=%%m
	if [%OVERWRITE%] == [] (
		if not exist "%OUT_DIR%%%m.map" (call :export) else (echo Skipping %%m ...)
	) else (
		call :export
	)
))

(for %%m in (%ZM_MAPS%) do (
	set MAP=%%m
	if [%OVERWRITE%] == [] (
		if not exist "%OUT_DIR%%%m.map" (call :export) else (echo Skipping %%m ...)
	) else (
		call :export
	)
))

set OUT_DIR=%GAME_DIR%\map_source\_prefabs\maps\mp\
if not exist %OUT_DIR% mkdir %OUT_DIR%
(for %%m in (%MP_MAPS%) do (
	set MAP=%%m
	if [%OVERWRITE%] == [] (
		if not exist "%OUT_DIR%%%m.map" (call :export) else (echo Skipping %%m ...)
	) else (
		call :export
	)
))

goto:eof

:export
echo Exporting %MAP% ...
"%ASSET_UTIL%" ents "%FF_DIR%%MAP%.ff" >> "%OUT_DIR%%MAP%.map"

:: If Asset Util did not exit correctly
::  we need to delete the map file
IF %ERRORLEVEL% NEQ 0 (
	echo ERROR^(%ERRORLEVEL%^): Deleting file...
	del "%OUT_DIR%%MAP%.map"
)

::
::cuba
::vorkuta
::pentagon
::flashpoint
::khe_sanh
::hue_city
::kowloon
::fullahead
::creek_1
::river
::wmd_sr71
::wmd
::pow
::rebirth
::int_escape
::underwaterbase
::
::zombie_theater
::zombie_pentagon
::zombietron
::zombie_cosmodrome
::zombie_coast
::zombie_temple
::zombie_moon
::zombie_cod5_asylum
::zombie_cod5_factory
::zombie_cod5_prototype
::zombie_cod5_sumpf
::
::mp_area51
::mp_array
::mp_berlinwall2
::mp_cairo
::mp_cosmodrome
::mp_cracked
::mp_crisis
::mp_discovery
::mp_drivein
::mp_duga
::mp_firingrange
::mp_golfcourse
::mp_gridlock
::mp_hanoi
::mp_havoc
::mp_hotel
::mp_kowloon
::mp_mountain
::mp_nuked
::mp_outskirts
::mp_radiation
::mp_russianbase
::mp_silo
::mp_stadium
::mp_villa
::mp_zoo
