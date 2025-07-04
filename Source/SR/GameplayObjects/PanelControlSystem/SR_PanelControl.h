// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SR/GameplayObjects/PlatformSystem/SR_BridgePlatform.h"
#include "SR_PanelControl.generated.h"

class ASR_Character;

UCLASS()
class SR_API ASR_PanelControl : public AActor
{
	GENERATED_BODY()

public:
	ASR_PanelControl();

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Panel Control")
	TArray<ASR_BridgePlatform *> ControlledPlatforms;

	UFUNCTION(BlueprintCallable, Category = "Panel Control")
	void TogglePanel();

	UFUNCTION(BlueprintPure, Category = "Panel Control")
	bool IsPanelActive() const;

	UPROPERTY()
	ACharacter *Character;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Panel Control")
	class UBoxComponent *CollisionBox;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp,
						int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp,
					  int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, Category = "Panel Control")
	void WantActivatePanel();

	UFUNCTION(BlueprintCallable, Category = "Panel Control")
	void TryActivatePanel();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Panel Control")
	float ActivationDistance = 200.0f;

	UFUNCTION(BlueprintPure, Category = "Panel Control")
	bool ArePlatformsIdle() const;

	UFUNCTION(BlueprintCallable, Category = "Panel Control")
	void ResetPanel();

	UFUNCTION(BlueprintCallable, Category = "Panel Control")
	void LockPanel();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Panel Control")
	UStaticMeshComponent *PanelMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Panel Control")
	bool bIsActivated;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Panel Control")
	bool bIsLocked;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Panel Control")
	UMaterialInterface *ActiveMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Panel Control")
	UMaterialInterface *InactiveMaterial;

	UPROPERTY(BlueprintReadOnly, Category = "Panel Control")
	bool bPlayerInRange;

	UPROPERTY(BlueprintReadOnly, Category = "Panel Control")
	bool bIsBusy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Panel Control")
	float CooldownTime = 1.5f;

	FTimerHandle CooldownTimerHandle;

	void OnCooldownEnd();

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	ASR_Character *OwnerCharacter;
};
