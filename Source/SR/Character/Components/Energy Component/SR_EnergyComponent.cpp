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

    // Détecter les objets interactifs à chaque frame
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

float USR_EnergyComponent::GetEnergyPercentage() const
{
    return static_cast<float>(CurrentEnergySlots) / static_cast<float>(MaxEnergySlots);
}

void USR_EnergyComponent::DetectInteractableObjects()
{
    if (!OwningActor || !OwningActor->GetWorld())
        return;

    // Paramètres pour la sphere trace
    FVector Start = OwningActor->GetActorLocation();
    TArray<FHitResult> HitResults;
    FCollisionShape Sphere = FCollisionShape::MakeSphere(DetectionRadius);
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(OwningActor);

    // Effectuer la sphere trace
    bool bHit = GetWorld()->SweepMultiByChannel(
        HitResults,
        Start,
        Start,
        FQuat::Identity,
        ECC_Visibility,
        Sphere,
        QueryParams
    );

    // Debug dessin de la sphère de détection
    #if WITH_EDITOR
        DrawDebugSphere(GetWorld(), Start, DetectionRadius, 12, FColor::Green, false, -1.0f, 0, 1.0f);
    #endif

    // Traiter les résultats
    if (bHit)
    {
        for (const FHitResult& Hit : HitResults)
        {
            if (AActor* HitActor = Hit.GetActor())
            {
                // Vérifier si c'est un pickup d'énergie 
                if (HitActor->ActorHasTag(FName("EnergyPickup")))
                {
                    AddEnergySlot();
                    HitActor->Destroy(); // Détruire le pickup après l'avoir ramassé
                }
                // Vérifier si c'est un objet interactable
                else if (HitActor->ActorHasTag(FName("Interactable")))
                {
                    // L'objet est dans la portée et peut être activé
                    // La logique d'interaction spécifique peut être implémentée ici ou via Blueprint
                }
            }
        }
    }
}

bool USR_EnergyComponent::IsInRange(AActor* OtherActor) const
{
    if (!OwningActor || !OtherActor)
        return false;

    float Distance = FVector::Distance(OwningActor->GetActorLocation(), OtherActor->GetActorLocation());
    return Distance <= DetectionRadius;
}