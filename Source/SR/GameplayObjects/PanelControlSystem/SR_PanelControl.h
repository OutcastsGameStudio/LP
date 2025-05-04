// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SR/GameplayObjects/PlatformSystem/SR_BridgePlatform.h"
#include "SR_PanelControl.generated.h"

UCLASS()
class SR_API ASR_PanelControl : public AActor
{
	GENERATED_BODY()

public:
	ASR_PanelControl();

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Panel Control")
	TArray<ASR_BridgePlatform*> ControlledPlatforms;

	UFUNCTION(BlueprintCallable, Category = "Panel Control")
	void TogglePanel();

	UFUNCTION(BlueprintPure, Category = "Panel Control")
	bool IsPanelActive() const;

	UPROPERTY()
	ACharacter* Character;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Panel Control")
	class UBoxComponent* CollisionBox;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
					   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
					   bool bFromSweep, const FHitResult& SweepResult);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Panel Control")
	UStaticMeshComponent* PanelMeshComponent;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Panel Control")
	bool bIsActivated;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Panel Control")
	UMaterialInterface* ActiveMaterial;
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Panel Control")
	UMaterialInterface* InactiveMaterial;

public:
	virtual void Tick(float DeltaTime) override;
};
