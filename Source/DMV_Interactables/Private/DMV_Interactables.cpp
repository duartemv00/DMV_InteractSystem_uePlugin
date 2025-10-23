// Copyright Epic Games, Inc. All Rights Reserved.

#include "DMV_Interactables.h"

#define LOCTEXT_NAMESPACE "FDMV_InteractablesModule"

void FDMV_InteractablesModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FDMV_InteractablesModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FDMV_InteractablesModule, DMV_Interactables)