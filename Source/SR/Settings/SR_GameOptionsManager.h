#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/Engine.h"
#include "GameFramework/GameUserSettings.h"
#include "SR_GameOptionsManager.generated.h"

UCLASS(BlueprintType, Blueprintable)
class SR_API USR_GameOptionsManager : public UObject
{
	GENERATED_BODY()

public:
	// Fonctions de sauvegarde et chargement
	UFUNCTION(BlueprintCallable, Category = "Game Options")
	static void SaveGameOptions(float Sensitivity, FIntPoint Resolution, TEnumAsByte<EWindowMode::Type> WindowMode, float Volume);
    
	UFUNCTION(BlueprintCallable, Category = "Game Options")
	static void LoadGameOptions(float& Sensitivity, FIntPoint& Resolution, TEnumAsByte<EWindowMode::Type>& WindowMode, float& Volume);
    
	// Fonction pour sauvegarder les contrôles
	UFUNCTION(BlueprintCallable, Category = "Game Options")
	static void SaveControlBinding(const FString& ActionName, const FString& KeyName);
    
	UFUNCTION(BlueprintCallable, Category = "Game Options")
	static FString LoadControlBinding(const FString& ActionName);
};