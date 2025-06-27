// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "BaseGameInstance.generated.h"

	/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class SR_API UBaseGameInstance : public UGameInstance{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<class UUserWidget> LoadingWidgetClass;

public:

	virtual void Init() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnMapBeginedOpen(const FString& String);
	virtual void OnMapBeginedOpen_Implementation(const FString& String);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnMapFinishedOpen(UWorld* World);
	virtual void OnMapFinishedOpen_Implementation(UWorld* World);
};
