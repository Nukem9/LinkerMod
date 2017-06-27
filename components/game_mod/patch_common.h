#pragma once

//
// Allows use of "com_configReadOnly" to prevent overwriting the config file with launch args
//
void Patch_WriteToConfig();

//
// Run any demo compatibility patches
//
void Patch_Demos();
