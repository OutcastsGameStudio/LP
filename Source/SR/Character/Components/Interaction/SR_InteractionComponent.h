#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "SR_InteractionComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SR_API USR_InteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USR_InteractionComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction *ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	AActor *GetDetectedInteractibleActor() const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float MaxDetectionDistance = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float CenterScreenTolerance = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bShowDebug = false;

	UPROPERTY(EditAnywhere, Category = "Interaction",
			  meta = (ClampMin = "0.0",
					  ToolTip = "Interval (s) between refreshes of the cached interactible actor list. The expensive "
								"GetAllActorsWithTag scan runs at this rate instead of every frame."))
	float ActorListRefreshInterval = 1.0f;

private:
	UPROPERTY()
	AActor *DetectedActor;

	// Cache de la liste des acteurs taggés "interactible", rafraîchie périodiquement
	// au lieu d'un GetAllActorsWithTag par frame.
	UPROPERTY()
	TArray<AActor *> CachedInteractibles;

	float TimeSinceActorListRefresh = 0.0f;

	void RefreshInteractibleActors();
	void FindInteractibleActorInView();
	bool IsActorInViewAndRange(const AActor *Actor, const FVector &CameraLocation, const FVector &CameraForward,
							   float &OutDistanceFromCenter);
};
