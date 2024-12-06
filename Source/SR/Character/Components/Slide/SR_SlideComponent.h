// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SR_SlideComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SR_API USR_SlideComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USR_SlideComponent();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StartSlide();

	void StopSlide();

	UFUNCTION()
	void ProcessSlide(float DeltaTime);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UCapsuleComponent* CapsuleComponent;
	UMeshComponent* MeshComponent;
	UCharacterMovementComponent* CharacterMovement;

	float fInitialCapsuleHalfHeight;
	bool bIsSliding = false;
	float fCapsuleHalfHeightSliding = 40.0f;
	float fBrakingDecelerationWalking;
	float fMeshLocationZ;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SlideDistance = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SlideDuration = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SlideSpeed = 1800.0f;

	FVector SlideStartLocation;
	FVector SlideDirection;
	float CurrentSlideDistance = 0.0f;
	
};
