[Code]

function  GetGamePath(): PChar;
external 'LMI_GamePath@files:installer.dll stdcall';

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
