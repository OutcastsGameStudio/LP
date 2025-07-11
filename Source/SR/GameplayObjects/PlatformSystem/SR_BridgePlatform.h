// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SR_BridgePlatform.generated.h"

UCLASS()
class SR_API ASR_BridgePlatform : public AActor
{
	GENERATED_BODY()

public:
	ASR_BridgePlatform();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void ActivateMovement(bool bShouldActivate);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void ResetPlatform();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void LockPlatform();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent *PlatformMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bIsMoving;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bIsActivated;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bIsLocked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bShouldRotate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bHasReachedDestination = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (EditCondition = "!bShouldRotate"))
	FVector EndPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (EditCondition = "bShouldRotate"))
	FRotator EndRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	class USoundBase *ActivationSound;

	float Alpha = 0;

public:
	UFUNCTION(BlueprintPure, Category = "Platform")
	bool IsMoving() const;

	virtual void Tick(float DeltaTime) override;

	FVector StartPosition;
	FRotator StartRotation;
	bool bIsReverse = false;
	FVector TargetPosition;
	FRotator TargetRotation;

private:
	FVector OriginLocation;
	FRotator OriginRotation;
};
