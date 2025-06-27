// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameInstance.h"

#include "Blueprint/UserWidget.h"
#include "Runtime/MoviePlayer/Public/MoviePlayer.h"

void UBaseGameInstance::Init()
{
	Super::Init();

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UBaseGameInstance::OnMapBeginedOpen);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UBaseGameInstance::OnMapFinishedOpen);
}

void UBaseGameInstance::OnMapBeginedOpen_Implementation(const FString& String){
	
	FLoadingScreenAttributes LoadingScreen;
	LoadingScreen.bAutoCompleteWhenLoadingCompletes = true;
	LoadingScreen.MinimumLoadingScreenDisplayTime = 2.0f;
	UUserWidget* LoadingWidget = CreateWidget<UUserWidget>(this, LoadingWidgetClass);
	LoadingScreen.WidgetLoadingScreen = LoadingWidget->TakeWidget();

	GEngine->AddOnScreenDebugMessage(1, 20.0f, FColor::Green, TEXT("Loading Screen Setup"));
	
	GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
}
void UBaseGameInstance::OnMapFinishedOpen_Implementation(UWorld* World){
	GetMoviePlayer()->StopMovie();
}
