// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SR/Character/Components/SR_CharacterMovementComponent.h"
#include "SR_DebugComponent.generated.h"

class ASR_Character;

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom),
       meta = (BlueprintSpawnableComponent))
class SR_API USR_DebugComponent : public UActorComponent {
  GENERATED_BODY()

public:
  // Sets default values for this component's properties
  USR_DebugComponent();

protected:
  // Called when the game starts
  virtual void BeginPlay() override;

public:
  // Called every frame
  virtual void
  TickComponent(float DeltaTime, ELevelTick TickType,
                FActorComponentTickFunction *ThisTickFunction) override;

  UFUNCTION(BlueprintCallable, Category = "Debug|Ledge Grab")
  void SetCharacterLedgeGrabReachDistance(float NewReachDistance);

  UFUNCTION(BlueprintCallable, Category = "Debug|Ledge Grab")
  float GetCharacterLedgeGrabReachDistance();

  UFUNCTION(BlueprintCallable, Category = "Debug|Ledge Grab")
  void SetLedgeGrabHeight(float NewLedgeGrabHeight);

  UFUNCTION(BlueprintCallable, Category = "Debug|Ledge Grab")
  float GetLedgeGrabHeight();

  UFUNCTION(BlueprintCallable, Category = "Debug|Climb")
  void SetClimbUpSpeed(float NewClimbUpSpeed);

  UFUNCTION(BlueprintCallable, Category = "Debug|Climb")
  float GetClimbUpSpeed();

  UFUNCTION(BlueprintCallable, Category = "Debug|Wall Jump")
  float GetWallJumpSpeed();

  UFUNCTION(BlueprintCallable, Category = "Debug|Wall Jump")
  void SetWallJumpSpeed(float NewWallJumpSpeed);

  UFUNCTION(BlueprintCallable, Category = "Debug|Wall Run")
  float GetMaxAngleWallRun();

  UFUNCTION(BlueprintCallable, Category = "Debug|Wall Run")
  void SetMaxAngleWallRun(float NewMaxAngleWallRun);

  UFUNCTION(BlueprintCallable, Category = "Debug|Wall Run")
  float GetMaxAngleBeforeStop();

  UFUNCTION(BlueprintCallable, Category = "Debug|Wall Run")
  void SetMaxAngleBeforeStop(float NewMaxAngleBeforeStop);

  UFUNCTION(BlueprintCallable, Category = "Debug|Wall Run")
  float GetWallRunFallingAcceleration();

  UFUNCTION(BlueprintCallable, Category = "Debug|Wall Run")
  void SetWallRunFallingAcceleration(float NewWallRunFallingAcceleration);

  UPROPERTY(BlueprintReadOnly, Category = "Debug")
  TArray<FName> DebugProperties = {"CharacterLedgeGrabReachDistance",
                                   "LedgeGrabHeight",
                                   "ClimbUpSpeed",
                                   "WallJumpSpeed",
                                   "MaxAngleWallRun",
                                   "MaxAngleBeforeStop",
                                   "WallRunFallingAcceleration"};

  UFUNCTION(BlueprintCallable, Category = "Debug")
  void InitDataTable();

  UFUNCTION(BlueprintCallable, Category = "Debug")
  void SetValuesFromDataTable();

  UFUNCTION(BlueprintCallable, Category = "Debug")
  void UpdateTable(FName RowName, float NewValue);

  UFUNCTION(BlueprintCallable, Category = "Game Version")
  FString GetGameVersion();

  UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debug")
  UDataTable *DebugDataTable;

  UFUNCTION(BlueprintCallable, Category = "Debug")
  void LoadDataTable();

private:
  UPROPERTY()
  ASR_Character *OwnerCharacter;

  UPROPERTY()
  USR_CharacterMovementComponent *CharacterMovementComponent;
};