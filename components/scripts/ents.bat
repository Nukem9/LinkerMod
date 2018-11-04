:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: Automatically generate Radiant compatible map files from the ent strings of all game maps ::
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

@echo off

set GAME_DIR=..\..
set ASSET_UTIL=%GAME_DIR%\bin\asset_util.exe
set ASSET_UTIL_FLAGS=--dummyBrushes --overwrite

"%ASSET_UTIL%" ents %ASSET_UTIL_FLAGS% *
