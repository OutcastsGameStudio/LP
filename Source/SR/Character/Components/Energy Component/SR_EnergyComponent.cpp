#include "SR_EnergyComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"

USR_EnergyComponent::USR_EnergyComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    CurrentEnergySlots = 0;
}

void USR_EnergyComponent::BeginPlay()
{
    Super::BeginPlay();
    OwningActor = Cast<ACharacter>(GetOwner());
}

void USR_EnergyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Detect interactive objects every frame
    DetectInteractableObjects();
}

bool USR_EnergyComponent::UseEnergySlot()
{
    if (CurrentEnergySlots > 0)
    {
        CurrentEnergySlots--;
        return true;
    }
    return false;
}

void USR_EnergyComponent::AddEnergySlot()
{
    if (CurrentEnergySlots < MaxEnergySlots)
    {
        CurrentEnergySlots++;
    }
}

int32 USR_EnergyComponent::GetEnergySlotRemaining() const
{
    return MaxEnergySlots - CurrentEnergySlots;
}

bool USR_EnergyComponent::IsEnergyEmpty() const
{
    return CurrentEnergySlots == 0;
}

bool USR_EnergyComponent::IsEnergyFull() const
{
    return CurrentEnergySlots == MaxEnergySlots;
}

void USR_EnergyComponent::DetectInteractableObjects()
{
    if (!OwningActor || !OwningActor->GetWorld())
        return;

    // Parameters for the sphere trace
    FVector Start = OwningActor->GetActorLocation();
    TArray<FHitResult> HitResults;
    FCollisionShape Sphere = FCollisionShape::MakeSphere(DetectionRadius);
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(OwningActor);

    // Perform the sphere trace
    bool bHit = GetWorld()->SweepMultiByChannel(
        HitResults,
        Start,
        Start,
        FQuat::Identity,
        ECC_Visibility,
        Sphere,
        QueryParams
    );

    // Debug draw of the detection sphere
    #if WITH_EDITOR
        DrawDebugSphere(GetWorld(), Start, DetectionRadius, 12, FColor::Green, false, -1.0f, 0, 1.0f);
    #endif

    // Process the results
    if (bHit)
    {
        for (const FHitResult& Hit : HitResults)
        {
            if (AActor* HitActor = Hit.GetActor())
            {
                // Check if it's an energy pickup
                if (HitActor->ActorHasTag(FName("EnergyPickup")))
                {
                    AddEnergySlot();
                    HitActor->Destroy(); // Destroy the pickup after collecting it
                }
                // Check if it's an interactable object
                else if (HitActor->ActorHasTag(FName("Interactable")))
                {
                    // The object is within range and can be activated
                    // Specific interaction logic can be implemented here or via Blueprint
                }
            }
        }
    }
}

bool USR_EnergyComponent::IsInteractable(AActor* OtherActor) const
{
    return IsInRange(OtherActor);
}

bool USR_EnergyComponent::IsInRange(AActor* OtherActor) const
{
    if (!OwningActor || !OtherActor)
        return false;

    float Distance = FVector::Distance(OwningActor->GetActorLocation(), OtherActor->GetActorLocation());
    return Distance <= DetectionRadius;
}