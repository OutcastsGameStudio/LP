#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BillboardComponent.h"
#include "SR_GrapplePoint.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGrapplePointActivated, AActor*, GrappledByActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGrapplePointDeactivated);

UENUM(BlueprintType)
enum class EGrapplePointType : uint8
{
    Standard    UMETA(DisplayName = "Standard"),
    SwingOnly   UMETA(DisplayName = "Swing Only"),
    PullOnly    UMETA(DisplayName = "Pull Only"),
    OneTime     UMETA(DisplayName = "One Time Use")
};

UCLASS(BlueprintType, Blueprintable)
class SR_API ASR_GrapplePoint : public AActor
{
    GENERATED_BODY()
    
public:    
    ASR_GrapplePoint();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* MeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* DetectionSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UBillboardComponent* IconComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple Point")
    float DetectionRadius = 200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple Point")
    bool bIsActive = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple Point")
    bool bRequireDirectLineOfSight = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple Point")
    float MaxGrappleDistance = 3000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple Point")
    EGrapplePointType GrapplePointType = EGrapplePointType::Standard;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple Point")
    bool bAutoRespawn = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple Point", 
              meta = (EditCondition = "bAutoRespawn", EditConditionHides))
    float RespawnTime = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    UMaterialInterface* ActiveMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    UMaterialInterface* InactiveMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    UMaterialInterface* HighlightedMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    bool bShowIcon = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    float IconScale = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    class UParticleSystem* ActivationParticle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    class USoundBase* ActivationSound;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnGrapplePointActivated OnGrapplePointActivated;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnGrapplePointDeactivated OnGrapplePointDeactivated;

public:    
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Grapple Point")
    bool CanBeGrappled(const FVector& PlayerLocation) const;

    UFUNCTION(BlueprintCallable, Category = "Grapple Point")
    void SetActive(bool bNewActive);

    UFUNCTION(BlueprintCallable, Category = "Grapple Point")
    void SetHighlighted(bool bHighlight);

    UFUNCTION(BlueprintPure, Category = "Grapple Point")
    bool IsActive() const { return bIsActive; }

    UFUNCTION(BlueprintPure, Category = "Grapple Point")
    FVector GetGrappleLocation() const;

    UFUNCTION(BlueprintCallable, Category = "Grapple Point")
    void OnGrappleAttached(AActor* GrapplingActor);

    UFUNCTION(BlueprintCallable, Category = "Grapple Point")
    void OnGrappleDetached();

    UFUNCTION(BlueprintPure, Category = "Grapple Point")
    EGrapplePointType GetGrappleType() const { return GrapplePointType; }

private:
    void UpdateVisualState();
    bool HasClearLineOfSight(const FVector& FromLocation) const;
    void HandleRespawn();
    
    FTimerHandle RespawnTimerHandle;
    bool bIsHighlighted = false;
    bool bIsBeingUsed = false;
};