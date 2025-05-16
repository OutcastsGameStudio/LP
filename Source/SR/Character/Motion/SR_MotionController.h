// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/RootMotionSource.h"
#include "SR_MotionController.generated.h"

class USR_CharacterMovementComponent;

UENUM(BlueprintType)
enum class ERootMotionPriority : uint8
{
	Low = 10 UMETA(DisplayName = "Low"),
	Medium = 5 UMETA(DisplayName = "Medium"),
	High = 0 UMETA(DisplayName = "High"),
};

USTRUCT(BlueprintType)
struct FRootMotionRequest
{
	GENERATED_BODY()

	// Nom unique pour cette requête de mouvement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Root Motion",
			  meta = (ToolTip = "Unique name for this motion request"))
	FName MovementName = NAME_None;

	// Direction et force du mouvement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Root Motion",
			  meta = (ToolTip = "Direction of the movement"))
	FVector Direction = FVector::ZeroVector;

	// Force/intensité du mouvement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Root Motion",
			  meta = (ToolTip = "Strength/intensity of the movement"))
	float Strength = 0.0f;

	// Durée du mouvement (0 pour instantané)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Root Motion",
			  meta = (ToolTip = "Duration of the movement (0 for instant)"))
	float Duration = 0.0f;

	// Si le mouvement est additif ou remplace complètement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Root Motion",
			  meta = (ToolTip = "Whether the movement is additive or completely replaces existing movement"))
	bool bIsAdditive = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Root Motion",
			  meta = (ToolTip = "Priority level of the root motion"))
	ERootMotionPriority Priority = ERootMotionPriority::Medium;

	// Courbe optionnelle pour l'intensité sur la durée
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Root Motion",
			  meta = (ToolTip = "Optional curve to control strength over time"))
	UCurveFloat *StrengthOverTime = nullptr;

	// Comportement en fin de mouvement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Root Motion",
			  meta = (ToolTip = "Behavior when the motion ends"))
	ERootMotionFinishVelocityMode VelocityOnFinish = ERootMotionFinishVelocityMode::ClampVelocity;

	// Vélocité à définir à la fin (si applicable)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Root Motion",
			  meta = (ToolTip = "Velocity to set at the end (if applicable)"))
	FVector SetVelocityOnFinish = FVector::ZeroVector;

	// Limiter la vélocité à cette valeur à la fin (si applicable)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Root Motion",
			  meta = (ToolTip = "Limit velocity to this value at the end (if applicable)"))
	float ClampVelocityOnFinish = 0.0f;

	// Activer la gravité pendant ce mouvement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Root Motion",
			  meta = (ToolTip = "Enable gravity during this movement"))
	bool bEnableGravity = true;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRootMotionCompleted, int32, RootMotionId, bool, bInterrupted);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SR_API USR_MotionController : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USR_MotionController();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction *ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	int32 ApplyRootMotion(const FRootMotionRequest &Request);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool CancelRootMotion(int32 RootMotionID);

	// Interrompre tous les mouvements RootMotion
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void CancelAllRootMotions();

	// Vérifier si un mouvement avec ce nom est actif
	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsRootMotionActive(FName MovementName);

	UPROPERTY(BlueprintAssignable, Category = "Movement|Events")
	FOnRootMotionCompleted OnRootMotionCompleted;

private:
	UPROPERTY()
	USR_CharacterMovementComponent *CharacterMovementComponent;

	UPROPERTY()
	ACharacter *OwnerCharacter;

	UPROPERTY()
	class USR_ContextStateComponent *ContextStateComponent;

	struct FActiveRootMotion
	{
		int32 ID;
		FName Name;
		float StartTime;
		float Duration;
	};

	// Liste des mouvements actuellement actifs
	TArray<FActiveRootMotion> ActiveRootMotions;

	// Map des IDs vers les délégués pour les callbacks
	TMap<int32, FTimerHandle> RootMotionTimers;

	// Méthode de callback interne
	void OnRootMotionFinished(int32 RootMotionID, FName MovementName);
};
