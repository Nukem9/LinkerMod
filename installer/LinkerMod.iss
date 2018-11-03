; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#include "./scripts/common.iss"

[Setup]
AppName=LinkerMod
AppVersion=0.0.1
UninstallDisplayIcon={app}\LinkerMod.exe
WizardImageFile=C:\Users\SE2Dev\Pictures\dface_512x512.bmp
WizardSmallImageFile=C:\Users\SE2Dev\Pictures\dface_512x512.bmp


; Inlude the Inno Downloader scripts
#include ReadReg(HKEY_LOCAL_MACHINE,'Software\Sherlock Software\InnoTools\Downloader','ScriptPath','');

[Files]
; Source: "build\Release\game_mod.dll"; DestDir: "{app}"
; Source: "README.md"; DestDir: "{app}"; Flags: isreadme

[Icons]
Name: "{commondesktop}\Game Mod"; Filename: "{#BinDir}\BlackOps.exe"
; Name: "{group}\LinkerMod\Game_Mod"; Filename: "{#BinDir}\BlackOps.exe"

[Components]
Name: "GameMod";	Description: "Game Mod";	\
					Types: full compact custom;	\
					Flags: fixed

; exlusive flag makes them radio buttons
Name: "LinkerMod"; Description: "Mod Tools"; Types: full custom
Name: "LinkerMod\AssetUtil";	Description: "Asset Util"; Types: full;			Flags:
Name: "LinkerMod\AssetViewer";	Description: "AssetViewer Mod"; Types: full;	Flags:
Name: "LinkerMod\CoD2Map";		Description: "CoD2Map Mod"; Types: full;		Flags:
Name: "LinkerMod\CoD2Rad";		Description: "CoD2Rad Mod"; Types: full;		Flags:
Name: "LinkerMod\Linker";		Description: "Linker Mod"; Types: full;			Flags:
Name: "LinkerMod\Radiant";		Description: "Radiant Mod"; Types: full;		Flags:

Name: "Debug";		Description: "Debug"; Types: full;
; Flags:

[Tasks]
Name: extract;	Description: "Extract Assets";	\
				Components: LinkerMod\AssetUtil;
Name: extract\iwd; 	Description: "Extract I&WDs"; 				\
					Components: LinkerMod\AssetUtil;
Name: extract\iwd\img; 	Description: "Extract Images"; 			\
						Components: LinkerMod\AssetUtil
Name: extract\iwd\snd; 	Description: "Extract &Sounds"; 		\
						Components: LinkerMod\AssetUtil;
Name: extract\iwd\misc; Description: "Extract &Other Assets"; 	\
						Components: LinkerMod\AssetUtil;

Name: extract\ffs; 		Description: "Extract &FastFiles"; 		\
						Components: LinkerMod\AssetUtil;
Name: extract\ffs\snd; 	Description: "Extract &Sounds"; 		\
						Components: LinkerMod\AssetUtil;
Name: extract\ffs\raw; 	Description: "Extract &Rawfiles"; 		\
						Components: LinkerMod\AssetUtil;
Name: extract\ffs\ents; Description: "Extract &Entity Maps"; 	\
						Components: LinkerMod\AssetUtil;

[Files]
;
; Actual LinkerMod binaries
;
Source: "build\Release\proxy.dll";			DestDir: "{#BinDir}";
Source: "build\Release\game_mod.dll";		DestDir: "{#BinDir}"; Components: GameMod
Source: "build\Release\asset_util.exe";		DestDir: "{#BinDir}"; Components: LinkerMod\AssetUtil
Source: "build\Release\asset_viewer.dll";	DestDir: "{#BinDir}"; Components: LinkerMod\AssetViewer
Source: "build\Release\cod2map.dll";		DestDir: "{#BinDir}"; Components: LinkerMod\CoD2Map
Source: "build\Release\cod2rad.dll";		DestDir: "{#BinDir}"; Components: LinkerMod\CoD2Rad
Source: "build\Release\linker_pc.dll";		DestDir: "{#BinDir}"; Components: LinkerMod\Linker
Source: "build\Release\radiant_mod.dll";	DestDir: "{#BinDir}"; Components: LinkerMod\Radiant

;
; Mod Tools asset files
;

; Utility scripts
Source: "components\scripts\*";		DestDir: "{#BinDir}\scripts";		\
									Components: LinkerMod\AssetUtil; 	\
									Flags: recursesubdirs;

; Custom / missing assets
; Source: "components\resource\*";	DestDir: "{app}";		\
;									Components: LinkerMod;	\
; 									Flags: recursesubdirs;

; Test automatic shit
; Source: "{code:GetAutoFiles}";				DestDir: "{#BinDir}\debug}";	Components: Debug; Flags: external recursesubdirs createallsubdirs

[Run]
;; extract-iwd --all --includeLocalized
Filename: "{#BinDir}\asset_util.exe";	StatusMsg: "Extracting IWD assets...";		\
										Parameters: "help";							\
										WorkingDir:	"{#BinDir}";					\
										Flags: runhidden;							\
										Tasks: extract\iwd;
;; extract-ff -v --all --includeLocalized
Filename: "{#BinDir}\asset_util.exe";	StatusMsg: "Extracting fastfile assets...";	\
										Parameters: "";								\
										WorkingDir:	"{#BinDir}";					\
										Flags: runhidden;							\
										Tasks: extract\ffs
; Filename: "{app}\README.TXT"; Description: "View the README file"; Flags: postinstall shellexec skipifsilent
; Filename: "{app}\MYPROG.EXE"; Description: "Launch application"; Flags: postinstall nowait skipifsilent unchecked

[Code]
// Test
var progress:TOutputProgressWizardPage;

procedure InitializeWizard;
// var
//	downloadPage:TWizardpage;
//	UserPage: TWizardPage;
//	ListBox: TNewListBox;
//	tags: TStringList;
//	i: Cardinal;
begin
 	itd_init;

	// tags := GetVersionTags();
//
	// // Attempt to get the installation path
	// UserPage :=  CreateCustomPage(wpWelcome, 'Which version should be installed?', '????');
//
//
	// ListBox := TNewListBox.Create(UserPage);
	// ListBox.Parent := UserPage.Surface;
//
	// For i := 0 to tags.Count - 1 do
	// begin
	//  ListBox.Items.Add(tags[i]);
	// end;
//
	// ListBox.Items.Add('test1');
	// ListBox.Items.Add('test2');
//
	// // GetString();
	//
	//  {Create our own progress page for the initial download of a small
	// 	textfile from the server which says what the latest version is}
	// //	progress := CreateOutputProgressPage(ITD_GetString(ITDS_Update_Caption), ITD_GetString(ITDS_Update_Description));
//
	// // Stuff
	// //Create the ITD GUI so that we have it if we decide to download a new intaller version
	// downloadPage:=itd_downloadafter(wpWelcome);
end;

function NextButtonClick(curPageID:integer): boolean;
begin
	Result := Com_ValidateInstallPath(curPageID);
end;
