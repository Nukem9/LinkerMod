@echo off

rem
rem ------- Find Visual Studio's install path -------
rem
:find2015
if "%VS140COMNTOOLS%"=="" (
    goto find2013
) else (
    echo Found: Visual Studio 2015
    set TMP_VSPATH="%VS140COMNTOOLS%\..\..\vc\vcvarsall.bat"
    goto compile
)

:find2013
if "%VS120COMNTOOLS%"=="" (
    goto find2012
) else (
    echo Found: Visual Studio 2013
    set TMP_VSPATH="%VS120COMNTOOLS%\..\..\vc\vcvarsall.bat"
    goto compile
)

:find2012
if "%VS110COMNTOOLS%"=="" (
    goto notfound
) else (
    echo Found: Visual Studio 2012
    set TMP_VSPATH="%VS110COMNTOOLS%\..\..\vc\vcvarsall.bat"
    goto compile
)

:notfound
echo Visual Studio (2012/2013/2015) wasn't found on your computer.
exit 1

rem
rem ------ Compile -------
rem
:compile
call %TMP_VSPATH% amd64_x86
set type=Configuration=Release;Platform=Win32

msbuild.exe LinkerMod.sln /maxcpucount /verbosity:minimal /t:Rebuild /p:%type%

rem
rem ------ Copy Files -------
rem
:docopy
setlocal ENABLEEXTENSIONS
set KEY_NAME=HKLM\SOFTWARE\activision\call of duty black ops
set VALUE_NAME=InstallPath

FOR /F "tokens=2*" %%A IN ('REG.exe query "%KEY_NAME%" /v "%VALUE_NAME%" /reg:32 ') DO (set pInstallDir=%%B)

if NOT "%pInstallDir%" == "" (
    copy .\build\Release\asset_viewer.dll "%pInstallDir%"\bin\asset_viewer.dll
    copy .\build\Release\cod2map.dll "%pInstallDir%"\bin\cod2map.dll
    copy .\build\Release\cod2rad.dll "%pInstallDir%"\bin\cod2rad.dll
    copy .\build\Release\game_mod.dll "%pInstallDir%"\bin\game_mod.dll
    copy .\build\Release\linker_pc.dll "%pInstallDir%"\bin\linker_pc.dll
    copy .\build\Release\path_mod.dll "%pInstallDir%"\bin\path_mod.dll
    copy .\build\Release\radiant_mod.dll "%pInstallDir%"\bin\radiant_mod.dll
    copy .\build\Release\launcher_ldr.exe "%pInstallDir%"\bin\launcher_ldr.exe
    exit 0
) else (
    echo InstallPath registry key wasn't found.
    exit 1
)