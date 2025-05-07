#include "SR_InteractionComponent.h"
#include "../../SR_Character.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

USR_InteractionComponent::USR_InteractionComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    DetectedActor = nullptr;
}

void USR_InteractionComponent::BeginPlay()
{
    Super::BeginPlay();
}

void USR_InteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    FindInteractibleActorInView();
}

bool USR_InteractionComponent::IsActorInViewAndRange(const AActor* Actor, const FVector& CameraLocation, 
    const FVector& CameraForward, float& OutDistanceFromCenter)
{
    if (!IsValid(Actor)) return false;

    // Vérifier la distance
    float Distance = FVector::Distance(CameraLocation, Actor->GetActorLocation());
    if (Distance > MaxDetectionDistance) return false;

    // Obtenir la position de l'acteur à l'écran
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!IsValid(PC)) return false;

    FVector2D ScreenLocation;
    if (!PC->ProjectWorldLocationToScreen(Actor->GetActorLocation(), ScreenLocation)) return false;

    // Obtenir les dimensions de l'écran
    int32 ViewportSizeX, ViewportSizeY;
    PC->GetViewportSize(ViewportSizeX, ViewportSizeY);

    // Calculer le centre de l'écran
    FVector2D ScreenCenter(ViewportSizeX * 0.5f, ViewportSizeY * 0.5f);

    // Normaliser la position sur l'écran (0-1)
    FVector2D NormalizedScreenLoc(
        ScreenLocation.X / ViewportSizeX,
        ScreenLocation.Y / ViewportSizeY
    );

    // Calculer la distance par rapport au centre de l'écran (0-1)
    OutDistanceFromCenter = FVector2D::Distance(
        NormalizedScreenLoc,
        FVector2D(0.5f, 0.5f)
    );

    // Vérifier si l'acteur est visible (pas obstrué)
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(GetOwner());
    QueryParams.AddIgnoredActor(Actor);

    bool bIsVisible = !GetWorld()->LineTraceSingleByChannel(
        HitResult,
        CameraLocation,
        Actor->GetActorLocation(),
        ECC_Visibility,
        QueryParams
    );

    return bIsVisible && OutDistanceFromCenter <= CenterScreenTolerance;
}

void USR_InteractionComponent::FindInteractibleActorInView()
{
    if (!IsValid(GetOwner())) return;
    
    ASR_Character* OwnerCharacter = Cast<ASR_Character>(GetOwner());
    if (!IsValid(OwnerCharacter)) return;

    UCameraComponent* FollowCamera = OwnerCharacter->GetFollowCamera();
    if (!IsValid(FollowCamera)) return;

    FVector CameraLocation = FollowCamera->GetComponentLocation();
    FVector CameraForward = FollowCamera->GetForwardVector();

    // Trouver tous les acteurs avec le tag "interactible"
    TArray<AActor*> InteractibleActors;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("interactible"), InteractibleActors);

    AActor* ClosestToCenterActor = nullptr;
    float ClosestDistanceToCenter = CenterScreenTolerance;

    // Trouver l'acteur le plus proche du centre de l'écran
    for (AActor* Actor : InteractibleActors)
    {
        float DistanceFromCenter;
        if (IsActorInViewAndRange(Actor, CameraLocation, CameraForward, DistanceFromCenter))
        {
            if (DistanceFromCenter < ClosestDistanceToCenter)
            {
                ClosestDistanceToCenter = DistanceFromCenter;
                ClosestToCenterActor = Actor;
            }
        }
    }

    // Simplement mettre à jour l'acteur détecté sans modifier son état
    DetectedActor = ClosestToCenterActor;

    // Debug
    if (bShowDebug && IsValid(DetectedActor))
    {
        DrawDebugLine(
            GetWorld(),
            CameraLocation,
            DetectedActor->GetActorLocation(),
            FColor::Green,
            false,
            -1.0f,
            0,
            2.0f
        );
    }
}

AActor* USR_InteractionComponent::GetDetectedInteractibleActor() const
{
    return DetectedActor;
}