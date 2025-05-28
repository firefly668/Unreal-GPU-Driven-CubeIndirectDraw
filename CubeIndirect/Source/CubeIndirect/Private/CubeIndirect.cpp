// Copyright Epic Games, Inc. All Rights Reserved.

#include "CubeIndirect.h"

#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FCubeIndirectModule"

void FCubeIndirectModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	const FString ShaderDirectory = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("CubeIndirect"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping("/CubeIndirectShaders", ShaderDirectory);
}


void FCubeIndirectModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCubeIndirectModule, CubeIndirect)