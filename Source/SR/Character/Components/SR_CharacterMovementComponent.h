// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SR_CharacterMovementComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SR_API USR_CharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USR_CharacterMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void WallJump();

	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	
	void UpdateWallRunDirection(FHitResult& Hit);
	
	bool DetectNextWall(FHitResult& Hit);

	FVector WallRunDirection;
	FVector WallNormal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRun")
	float WallJumpSpeed = 1000.f;
};
