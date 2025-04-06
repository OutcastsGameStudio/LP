// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <string>

#include "CoreMinimal.h"
#include "SR/Character/Components/SR_CharacterMovementComponent.h"

#include "SR_DebugComponent.generated.h"

class ASR_Character;


UCLASS(Blueprintable, BlueprintType,ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SR_API USR_DebugComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USR_DebugComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Ledge Grab Reach Distance")
	void SetCharacterLedgeGrabReachDistance(float NewReachDistance);

	UFUNCTION(BlueprintCallable, Category = "Ledge Grab Reach Distance")
	float GetCharacterLedgeGrabReachDistance();

	UFUNCTION(BlueprintCallable, Category = "Ledge Grab Height")
	void SetLedgeGrabHeight(float NewLedgeGrabHeight);

	UFUNCTION(BlueprintCallable, Category = "Ledge Grab Height")
	float GetLedgeGrabHeight();

	UFUNCTION(BlueprintCallable, Category = "Climb Up Speed")
	void SetClimbUpSpeed(float NewClimbUpSpeed);

	UFUNCTION(BlueprintCallable, Category = "Climb Up Speed")
	float GetClimbUpSpeed();

	UFUNCTION(BlueprintCallable, Category = "Ledge Grab Height")
	float GetWallJumpSpeed();

	UFUNCTION(BlueprintCallable, Category = "Ledge Grab Height")
	void SetWallJumpSpeed(float NewWallJumpSpeed);

	UFUNCTION(BlueprintCallable, Category = "Ledge Grab Height")
	float GetMaxAngleWallRun();

	UFUNCTION(BlueprintCallable, Category = "Ledge Grab Height")
	void SetMaxAngleWallRun(float NewMaxAngleWallRun);

	UFUNCTION(BlueprintCallable, Category = "Ledge Grab Height")
	float GetMaxAngleBeforeStop();

	UFUNCTION(BlueprintCallable, Category = "Ledge Grab Height")
	void SetMaxAngleBeforeStop(float NewMaxAngleBeforeStop);

	UFUNCTION(BlueprintCallable, Category = "Ledge Grab Height")
	float GetWallRunFallingAcceleration();

	UFUNCTION(BlueprintCallable, Category = "Ledge Grab Height")
	void SetWallRunFallingAcceleration(float NewWallRunFallingAcceleration);

	TArray<FName> DebugProps = { "CharacterLedgeGrabReachDistance", "LedgeGrabHeight", "ClimbUpSpeed", "WallJumpSpeed", "MaxAngleWallRun", "MaxAngleBeforeStop", "WallRunFallingAcceleration" };
	

	void InitDataTable();
	void SetValuesFromDataTable();

	void UpdateTable(FName RowName, float NewValue);




// Show variables
	UFUNCTION(BlueprintCallable, Category = "Is Wall RUn")
	bool GetIsCharacterWallRunning();


	UFUNCTION(BlueprintCallable, Category = "Is Wall Hanging")
	bool GetIsCharacterClimbing();
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UDataTable* DebugDataTable;

	void LoadDataTable();
private:
	ASR_Character* CharacterOwner;
	USR_CharacterMovementComponent* CharacterMovementComponent;
};
