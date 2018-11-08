#define ProjectGroup 'LinkerMod'
#define BinDir '{app}\bin'

#define PleaseWait '(this may take several minutes)'

#define GameMod_Exe	 "BlackOps.exe"

#include "./utility.iss"

[Setup]
; Require Windows Vista or higher
MinVersion=6.0

DefaultGroupName="{#ProjectGroup}"

#if BUILD_TYPE == 'PRODUCTION'
	LicenseFile="./installer/license.txt"
#endif

OutputDir=./build/
Compression=lzma2
SolidCompression=yes

DisableWelcomePage=no

; Install Path Options
DefaultDirName={code:GetDefaultDir}
AppendDefaultDirName=no
UsePreviousAppDir=no
DirExistsWarning=no

#if BUILD_TYPE == 'PRODUCTION'
	DisableDirPage=no
#else
	DisableDirPage=yes
#endif

[Languages]
Name: english; MessagesFile: compiler:Default.isl

[Registry]
; LinkerMod Registry Group
Root: HKLM; Subkey: "Software\{#ProjectGroup}"; \
			Flags: uninsdeletekeyifempty createvalueifdoesntexist;

[Files]
; Installer Bootstrap DLL (required for external dll code)
Source: ".\build\Release\bootstrap.dll";	DestDir: {app}; \
											Flags: dontcopy;

[Code]
(* Note: These *MUST* use the module exports definition files	*)
(*       Using __declspec(dllexport) does *NOT* work 			*)

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

//
// Resolve the game's installation path via the installation bootstrapper
// Returns an empty string on failure
//
function  GetGamePath(): PChar;
external 'LMI_GamePath@files:bootstrap.dll stdcall';

//
// Resolve the game's installation path via the installation bootstrapper
// Returns an empty string on failure
//
function  PE_AddNamedImport(filepath: string; _module: string; symbol: string): Integer;
external 'PE_AddNamedImport@files:bootstrap.dll stdcall';

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
function Pkg_CheckInstalledVersion(const packageName: string; var installedVersion: String ): boolean;
begin
	Result := RegQueryStringValue(HKEY_LOCAL_MACHINE,
						'Software\{#ProjectGroup}\' + packageName,
						'CurrentVersion',
						installedVersion);
end;

//
// Returns the installed version of a package, or an empty string if not installed
//
function Pkg_GetInstalledVersion(const packageName: string): string;
var
	installedVersion: string;
begin
	if not Pkg_CheckInstalledVersion(packageName, installedVersion) then
	begin
		// Return an empty string if the package isn't installed
		Result := #0;
		Exit;
	end

	Result := installedVersion;
end;

//
// Check if a package has an existing installation present
// Returns True if present
//
function Pkg_IsInstalled(const packageName: string): boolean;
var
	installedVersion: string;
begin
	Result := Pkg_CheckInstalledVersion(packageName, installedVersion);
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
	if not Pkg_CheckInstalledVersion(packageName, installedVersion) then
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

//
// Validate the install path
// Returns True if the path is valid
//
function Com_ValidateInstallPath(curPageID:integer): boolean;
begin
	Result := True;

	if (CurPageID = wpSelectDir) and (SetInstallPath(WizardDirValue) = false) then
	begin
		Result := False;
		MsgBox('Target installation directory is invalid. ' +
			'Choose a different one.', mbError, MB_OK);
		Exit;
	end;
end;

procedure PE_AddImport(moduleName: string; symbolName: string);
var
	oldStatus: string;
	oldFilename: string;
begin
	// Backup the current label contents
	oldStatus := WizardForm.StatusLabel.Caption;
	oldFilename := WizardForm.FilenameLabel.Caption;

	// Set the new label contents
	WizardForm.StatusLabel.Caption := 'Patching imports...';
	WizardForm.FilenameLabel.Caption := ExpandConstant(CurrentFileName);

	PE_AddNamedImport(ExpandConstant(CurrentFileName), moduleName, symbolName);

	// Restore the old contents
	WizardForm.StatusLabel.Caption := oldStatus;
	WizardForm.FilenameLabel.Caption := oldFilename;
end;
