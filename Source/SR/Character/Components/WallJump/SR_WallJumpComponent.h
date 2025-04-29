// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SR/Character/SR_Character.h"
#include "SR/Character/Components/ContextState/SR_ContextStateComponent.h"
#include "SR/Character/Components/ContextState/SR_State.h"
#include "SR_WallJumpComponent.generated.h"


class USR_MotionController;

USTRUCT()
struct FWallJumpData
{
	GENERATED_BODY()

	UPROPERTY()
	FVector WallRunDirection;

	UPROPERTY()
	FVector WallNormal;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SR_API USR_WallJumpComponent : public UActorComponent, public ISR_State
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USR_WallJumpComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;


public:
	virtual void EnterState(void* data) override;
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void LeaveState(int32 rootMotionId, bool bForced = false) override;
	virtual bool LookAheadQuery() override;
	virtual void UpdateState() override;
	virtual FName GetStateName() const override;
	virtual int32 GetStatePriority() const override;
	virtual bool IsStateActive() const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRun", meta = (ExposeOnSpawn = true))
	UCurveFloat* WallJumpStrengthCurve = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRun",meta = (ToolTip = "Durée maximale du wall run en secondes. Au-delà, le personnage tombe"))
	float WallJumpForce = 3.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRun",meta = (ToolTip = "Durée maximale du wall run en secondes. Au-delà, le personnage tombe"))
	float Duration = 3.f;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRun",meta = (ToolTip = "Durée maximale du wall run en secondes. Au-delà, le personnage tombe"))
    float WallJumpSpeed = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRun", 
	meta = (ToolTip = "Multiplicateur de la composante vers l'avant lors du saut depuis un wall run. Valeurs élevées = saut plus loin dans la direction de course. Valeur par défaut : 1.0"))
	float WallRunDirectionRatio = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRun", 
		meta = (ToolTip = "Multiplicateur de la composante perpendiculaire au mur lors du saut depuis un wall run. Valeurs élevées = saut plus loin du mur. Valeur par défaut : 1.0"))
	float WallRunNormalRatio = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallRun", 
		meta = (ToolTip = "Multiplicateur de la composante verticale lors du saut depuis un wall run. Valeurs élevées = saut plus haut. Valeur par défaut : 1.0"))
	float WallRunUpRatio = 1.0f;
private:
	UPROPERTY()
	UCharacterMovementComponent* CharacterMovement;
	UPROPERTY()
	ASR_Character* OwnerCharacter;
	UPROPERTY()
	USR_MotionController* MotionController;
	UPROPERTY()
	USR_ContextStateComponent* ContextStateComponent;


	UFUNCTION()
	void OnJumpButtonPressed();


	UFUNCTION()
	void OnJumpButtonReleased();


	FVector m_WallRunDirection;

	int32 m_WallRunMainMotionId = 0;

	FVector m_WallNormal;
};
