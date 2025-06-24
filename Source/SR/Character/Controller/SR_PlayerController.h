// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SR_PlayerController.generated.h"

class UInputMappingContext;

/**
 *
 */
UCLASS()
class SR_API ASR_PlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	/** Input Mapping Context to be used for player input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext *InputMappingContext;

	virtual void BeginPlay() override;

	// End Actor interface
};
