#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "SR_InteractionComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SR_API USR_InteractionComponent : public UActorComponent {
  GENERATED_BODY()

public:
  USR_InteractionComponent();
  virtual void
  TickComponent(float DeltaTime, ELevelTick TickType,
                FActorComponentTickFunction *ThisTickFunction) override;

  UFUNCTION(BlueprintCallable, Category = "Interaction")
  AActor *GetDetectedInteractibleActor() const;

protected:
  virtual void BeginPlay() override;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
  float MaxDetectionDistance = 1000.0f;

  // Tolérance de distance au centre de l'écran (0-1)
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction",
            meta = (ClampMin = "0.0", ClampMax = "1.0"))
  float CenterScreenTolerance = 0.3f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
  bool bShowDebug = false;

private:
  UPROPERTY()
  AActor *DetectedActor;

  void FindInteractibleActorInView();
  bool IsActorInViewAndRange(const AActor *Actor, const FVector &CameraLocation,
                             const FVector &CameraForward,
                             float &OutDistanceFromCenter);
};