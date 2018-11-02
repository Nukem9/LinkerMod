[Code]
(* Note: These *MUST* use the module exports definition files	*)
(*       Using __declspec(dllexport) does *NOT* work 			*)
function  TestFunc( buffer:PChar): Cardinal;
external 'TestFunc@files:bootstrap.dll stdcall';

//
// Compare to semantic version strings
// Returns <0 if 'a' is earlier than 'b'
// Returns  0 if 'a' is equal to 'b'
// Returns >0 if 'a' is newer than 'b'
//
function  CompareVersions(a: String; b: String): Integer;
external 'LMI_CompareVersions@files:bootstrap.dll stdcall';

//
// Attempt to set the target installation path for the installation bootstrapper
// Returns True if the path is a valid path
// Returns False if the path is not a valid path
//
function  SetInstallPath(path: String): Boolean;
external 'LMI_SetInstallPath@files:bootstrap.dll stdcall';

// DEPRECATED
function  GetVersions(): PChar;
external 'LMI_GetTags@files:bootstrap.dll stdcall';

//
// Resolve the game's installation path via the installation bootstrapper
// Returns an empty string on failure
//
function  GetGamePath(): PChar;
external 'LMI_GamePath@files:bootstrap.dll stdcall';

//
// Get the (automatically resolved) default installation directory
// Returns an empty string if it couldn't be found
//
// Serves as an extra safety wrapper around GetGamePath()
//
function GetDefaultDir(Param: string): string;
var
	installPath: PChar;
begin
	installPath := GetGamePath();
	if(length(installPath) < 1) then
		Result := #0
	else
		Result := installPath;
end;
