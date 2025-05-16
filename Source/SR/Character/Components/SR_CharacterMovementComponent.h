// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SR_CharacterMovementComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SR_API USR_CharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	USR_CharacterMovementComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction *ThisTickFunction) override;

public:
	enum CustomMode
	{
		CUSTOM_None = 0,
		CUSTOM_WallRun = 1,
		CUSTOM_DASH = 2,
	};
	void SetCustomMode(int32 NewCustomMode) { CustomMovementMode = (CustomMode)NewCustomMode; }

private:
	CustomMode CustomMovementMode;
	bool bIsMovingForward;
};
