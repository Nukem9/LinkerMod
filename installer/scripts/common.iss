#define ProjectGroup 'LinkerMod'

[Setup]
; Require Windows Vista or higher
MinVersion=6.0

DefaultGroupName="{#ProjectGroup}"
LicenseFile="./installer/license.txt"

OutputDir=./build/
Compression=lzma2
SolidCompression=yes

DisableWelcomePage=no

; Install Path Options
DefaultDirName={code:GetDefaultDir}
AppendDefaultDirName=no
UsePreviousAppDir=no
DirExistsWarning=no
DisableDirPage=No

[Languages]
Name: english; MessagesFile: compiler:Default.isl

[Registry]
; LinkerMod Registry Group
Root: HKLM; Subkey: "Software\{#ProjectGroup}"; \
			Flags: uninsdeletekeyifempty createvalueifdoesntexist;

[Code]
(* UTILITY FUNCTION - REMOVE FOR PRODUCTION *)
function BoolToStr(Value : Boolean) : String; 
begin
  if Value then
    result := 'true'
  else
    result := 'false';
end;


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

//
// Check if a package has an existing installation present
// Returns True if present, and assigns the installed version
// string to "installedVersion"
//
function Pkg_CheckIfInstalled(const packageName: string; var installedVersion: String): boolean;
begin
	Result := RegQueryStringValue(HKEY_LOCAL_MACHINE,
						'Software\{#ProjectGroup}\' + packageName,
						'CurrentVersion',
						installedVersion);
end;

//
// Ask the user if they would like to upgrade from one version
// of a package to another one.
//
// Returns True if the user said Yes
//
function Pkg_AskToUpgrade(packageName: string; fromVersion: string; toVersion: string): boolean;
begin
	Result := (MsgBox(	'An existing installation of ' + packageName + ' has been detected.' + #13#10 + #13#10 +
			'Installed: ' + packageName + ' (' + fromVersion + ')' + #13#10 + 
			'Bundled: ' + packageName + ' (' + toVersion + ')' + #13#10 + #13#10 +
			'Would you like to automatically install the bundled version of ' + packageName + '?' , mbConfirmation, MB_YESNO) = IDYES);
end;

//
// Check if an existing version of a package is installed, then check
// if the bundled version of that package is newer than the installed one.
// If the bundled version is newer, ask the user if they want to upgrade
// to the bundled version.
//
// Returns: True if the package should be updated
//
function Pkg_CheckForUpgrade(packageName: string; bundledVersion: string): boolean;
var
	installedVersion: String;
begin
	(* Check for existing <packageName> installs. If there is no 
	   existing installation of <packageName>, we need to install
	   the version bundled with the ModTools installer *)
	if not Pkg_CheckIfInstalled(packageName, installedVersion) then
	begin
		Result := True;
		Exit;
	end;

	(*
	   Check if the bundled version of <packageName> is newer than
	   the existing version. If the bundled version is newer
	   we need update <packageName> to use the bundled one.
	*)
	if CompareVersions(bundledVersion, installedVersion) > 0 then
	begin
		Result := Pkg_AskToUpgrade(packageName, installedVersion, bundledVersion);
		Exit;
	end

	Result := False;
end;
