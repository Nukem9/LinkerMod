[Code]
(* Note: These *MUST* use the module exports definition files	*)
(*       Using __declspec(dllexport) does *NOT* work 			*)
function  TestFunc( buffer:PChar): Cardinal;
external 'TestFunc@files:installer.dll stdcall';

function  GetGamePath(): PChar;
external 'LMI_GamePath@files:installer.dll stdcall';

function  SetInstallPath(path: String): Boolean;
external 'LMI_SetInstallPath@files:installer.dll stdcall';

function  GetVersions(): PChar;
external 'LMI_GetTags@files:installer.dll stdcall';

//
// Get the default installation directory
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
