// Fill out your copyright notice in the Description page of Project Settings.


#include "SR_GameOptionsManager.h"
#include "Kismet/GameplayStatics.h"

void USR_GameOptionsManager::SaveGameOptions(float Sensitivity, FIntPoint Resolution, TEnumAsByte<EWindowMode::Type> WindowMode, float GeneralVolume, float MusicVolume, float GameplayVolume, float AmbienceVolume)
{
    // Sauvegarder la sensibilité
    GConfig->SetFloat(TEXT("UserOptions"), TEXT("MouseSensitivity"), Sensitivity, GGameUserSettingsIni);
    
    // Sauvegarder la résolution
    GConfig->SetInt(TEXT("UserOptions"), TEXT("ResolutionX"), Resolution.X, GGameUserSettingsIni);
    GConfig->SetInt(TEXT("UserOptions"), TEXT("ResolutionY"), Resolution.Y, GGameUserSettingsIni);
    
    // Sauvegarder le mode de fenêtre
    int32 WindowModeInt = static_cast<int32>(WindowMode.GetValue());
    GConfig->SetInt(TEXT("UserOptions"), TEXT("WindowMode"), WindowModeInt, GGameUserSettingsIni);
    
    // Sauvegarder le volume
    GConfig->SetFloat(TEXT("UserOptions"), TEXT("GeneralVolume"), GeneralVolume, GGameUserSettingsIni);
	GConfig->SetFloat(TEXT("UserOptions"), TEXT("MusicVolume"), MusicVolume, GGameUserSettingsIni);
	GConfig->SetFloat(TEXT("UserOptions"), TEXT("GameplayVolume"), GameplayVolume, GGameUserSettingsIni);
	GConfig->SetFloat(TEXT("UserOptions"), TEXT("AmbienceVolume"), AmbienceVolume, GGameUserSettingsIni);
    
    // Appliquer les changements au fichier
    GConfig->Flush(false, GGameUserSettingsIni);
    
    UE_LOG(LogTemp, Log, TEXT("Options sauvegardées"));
}

void USR_GameOptionsManager::LoadGameOptions(float& Sensitivity, FIntPoint& Resolution, TEnumAsByte<EWindowMode::Type>& WindowMode, float& GeneralVolume, float& MusicVolume, float& GameplayVolume, float& AmbienceVolume)
{
    // Charger la sensibilité (valeur par défaut : 1.0)
    if (!GConfig->GetFloat(TEXT("UserOptions"), TEXT("MouseSensitivity"), Sensitivity, GGameUserSettingsIni))
    {
        Sensitivity = 1.0f;
    }
    
    // Charger la résolution (valeur par défaut : 1920x1080)
    int32 ResX, ResY;
    if (!GConfig->GetInt(TEXT("UserOptions"), TEXT("ResolutionX"), ResX, GGameUserSettingsIni))
    {
        ResX = 1920;
    }
    if (!GConfig->GetInt(TEXT("UserOptions"), TEXT("ResolutionY"), ResY, GGameUserSettingsIni))
    {
        ResY = 1080;
    }
    Resolution = FIntPoint(ResX, ResY);
    
    // Charger le mode de fenêtre (valeur par défaut : Fullscreen)
    int32 WindowModeInt;
    if (!GConfig->GetInt(TEXT("UserOptions"), TEXT("WindowMode"), WindowModeInt, GGameUserSettingsIni))
    {
        WindowModeInt = static_cast<int32>(EWindowMode::Fullscreen);
    }
    WindowMode = static_cast<EWindowMode::Type>(WindowModeInt);
    
    // Charger le volume (valeur par défaut : 1.0)
    if (!GConfig->GetFloat(TEXT("UserOptions"), TEXT("GeneralVolume"), GeneralVolume, GGameUserSettingsIni))
    {
        GeneralVolume = 1.0f;
    }
	if (!GConfig->GetFloat(TEXT("UserOptions"), TEXT("MusicVolume"), MusicVolume, GGameUserSettingsIni))
	{
		MusicVolume = 1.0f;
	}
	if (!GConfig->GetFloat(TEXT("UserOptions"), TEXT("GameplayVolume"), GameplayVolume, GGameUserSettingsIni))
	{
		GameplayVolume = 1.0f;
	}
	if (!GConfig->GetFloat(TEXT("UserOptions"), TEXT("AmbienceVolume"), AmbienceVolume, GGameUserSettingsIni))
	{
		AmbienceVolume = 1.0f;
	}

		
    
    UE_LOG(LogTemp, Log, TEXT("Options chargées - Sensibilité: %f, Résolution: %dx%d, GeneralVolume: %f, MusicVolume: %f, GameplayVolume: %f, AmbienceVolume: %f"), 
           Sensitivity, Resolution.X, Resolution.Y, GeneralVolume, MusicVolume, GameplayVolume, AmbienceVolume);
}

void USR_GameOptionsManager::SaveControlBinding(const FString& ActionName, const FString& KeyName)
{
    FString SectionName = TEXT("ControlBindings");
    GConfig->SetString(*SectionName, *ActionName, *KeyName, GGameUserSettingsIni);
    GConfig->Flush(false, GGameUserSettingsIni);
}

FString USR_GameOptionsManager::LoadControlBinding(const FString& ActionName)
{
    FString KeyName;
    FString SectionName = TEXT("ControlBindings");
    GConfig->GetString(*SectionName, *ActionName, KeyName, GGameUserSettingsIni);
    return KeyName;
}