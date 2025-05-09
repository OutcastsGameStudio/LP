<<<<<<< HEAD
// Copyright Epic Games, Inc. All Rights Reserved.

#include "FMODStudioNiagara.h"
#include "Misc/Paths.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FFMODStudioNiagaraModule"

void FFMODStudioNiagaraModule::StartupModule()
{
    IPluginManager::Get().FindPlugin(TEXT("FMODStudioNiagara"))->GetBaseDir();
}

void FFMODStudioNiagaraModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
=======
// Copyright Epic Games, Inc. All Rights Reserved.

#include "FMODStudioNiagara.h"
#include "Misc/Paths.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FFMODStudioNiagaraModule"

void FFMODStudioNiagaraModule::StartupModule()
{
    IPluginManager::Get().FindPlugin(TEXT("FMODStudioNiagara"))->GetBaseDir();
}

void FFMODStudioNiagaraModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
>>>>>>> e5e329b (fmod)
IMPLEMENT_MODULE(FFMODStudioNiagaraModule, FMODStudioNiagara)