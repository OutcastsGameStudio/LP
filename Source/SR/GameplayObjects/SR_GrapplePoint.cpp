#include "SR_GrapplePoint.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BillboardComponent.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ASR_GrapplePoint::ASR_GrapplePoint()
{
    PrimaryActorTick.bCanEverTick = true;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(RootComponent);
    MeshComponent->SetCollisionProfileName(TEXT("BlockAll"));

    DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
    DetectionSphere->SetupAttachment(RootComponent);
    DetectionSphere->SetSphereRadius(DetectionRadius);
    DetectionSphere->SetCollisionProfileName(TEXT("Trigger"));
    DetectionSphere->SetGenerateOverlapEvents(false);
    DetectionSphere->SetCanEverAffectNavigation(false);

    IconComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("IconComponent"));
    IconComponent->SetupAttachment(RootComponent);
    IconComponent->SetRelativeScale3D(FVector(IconScale));
    IconComponent->SetHiddenInGame(!bShowIcon);

    Tags.Add(FName("GrapplePoint"));

    SetActorTickEnabled(true);
}

void ASR_GrapplePoint::BeginPlay()
{
    Super::BeginPlay();
    
    if (DetectionSphere)
    {
        DetectionSphere->SetSphereRadius(DetectionRadius);
    }

    if (IconComponent)
    {
        IconComponent->SetRelativeScale3D(FVector(IconScale));
        IconComponent->SetHiddenInGame(!bShowIcon);
    }

    UpdateVisualState();
}

void ASR_GrapplePoint::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

bool ASR_GrapplePoint::CanBeGrappled(const FVector& PlayerLocation) const
{
    if (!bIsActive || bIsBeingUsed)
    {
        return false;
    }

    float Distance = FVector::Distance(PlayerLocation, GetActorLocation());
    if (Distance > MaxGrappleDistance)
    {
        return false;
    }

    if (bRequireDirectLineOfSight)
    {
        if (!HasClearLineOfSight(PlayerLocation))
        {
            return false;
        }
    }

    return true;
}

void ASR_GrapplePoint::SetActive(bool bNewActive)
{
    if (bIsActive != bNewActive)
    {
        bIsActive = bNewActive;
        UpdateVisualState();
        
        if (!bIsActive)
        {
            OnGrapplePointDeactivated.Broadcast();
        }
    }
}

void ASR_GrapplePoint::SetHighlighted(bool bHighlight)
{
    if (bIsHighlighted != bHighlight)
    {
        bIsHighlighted = bHighlight;
        UpdateVisualState();
    }
}

FVector ASR_GrapplePoint::GetGrappleLocation() const
{
    return GetActorLocation();
}

void ASR_GrapplePoint::OnGrappleAttached(AActor* GrapplingActor)
{
    if (!bIsActive)
    {
        return;
    }

    bIsBeingUsed = true;

    if (ActivationParticle)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ActivationParticle, GetActorLocation());
    }

    if (ActivationSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, ActivationSound, GetActorLocation());
    }

    OnGrapplePointActivated.Broadcast(GrapplingActor);

    if (GrapplePointType == EGrapplePointType::OneTime)
    {
        SetActive(false);
        
        if (bAutoRespawn)
        {
            GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, 
                &ASR_GrapplePoint::HandleRespawn, RespawnTime, false);
        }
    }
}

void ASR_GrapplePoint::OnGrappleDetached()
{
    bIsBeingUsed = false;
    UpdateVisualState();
}

void ASR_GrapplePoint::UpdateVisualState()
{
    if (!MeshComponent)
    {
        return;
    }

    UMaterialInterface* MaterialToUse = nullptr;

    if (!bIsActive)
    {
        MaterialToUse = InactiveMaterial;
        if (IconComponent)
        {
            IconComponent->SetHiddenInGame(true);
        }
    }
    else if (bIsHighlighted)
    {
        MaterialToUse = HighlightedMaterial ? HighlightedMaterial : ActiveMaterial;
        if (IconComponent && bShowIcon)
        {
            IconComponent->SetHiddenInGame(false);
        }
    }
    else
    {
        MaterialToUse = ActiveMaterial;
        if (IconComponent && bShowIcon)
        {
            IconComponent->SetHiddenInGame(false);
        }
    }

    if (MaterialToUse)
    {
        MeshComponent->SetMaterial(0, MaterialToUse);
    }

    if (bIsActive)
    {
        MeshComponent->SetCollisionProfileName(TEXT("BlockAll"));
    }
    else
    {
        MeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
    }
}

bool ASR_GrapplePoint::HasClearLineOfSight(const FVector& FromLocation) const
{
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    QueryParams.bTraceComplex = false;

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        FromLocation,
        GetActorLocation(),
        ECC_Visibility,
        QueryParams
    );

    return !bHit;
}

void ASR_GrapplePoint::HandleRespawn()
{
    SetActive(true);
    bIsBeingUsed = false;
    UpdateVisualState();
}