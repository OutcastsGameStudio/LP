// Fill out your copyright notice in the Description page of Project Settings.

#include "SR_DashComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

USR_DashComponent::USR_DashComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    
    // Valeurs par défaut configurables
    DashDistance = 1000.0f;     // Distance de dash en cm
    DashSpeed = 3000.0f;         // Vitesse de dash rapide
    DashDuration = 0.2f;         // Durée du dash
    bCanDash = true;             // Dash disponible par défaut
    DashCooldown = 1.0f;         // Temps de recharge entre deux dashs
}

void USR_DashComponent::BeginPlay()
{
    Super::BeginPlay();

    // Récupérer les composants nécessaires
    CharacterMovement = GetOwner()->FindComponentByClass<UCharacterMovementComponent>();
    OwnerCharacter = Cast<ACharacter>(GetOwner());

    if (!CharacterMovement || !OwnerCharacter)
    {
        UE_LOG(LogTemp, Error, TEXT("Dash Component: Failed to get Character or CharacterMovement"));
    }
}

void USR_DashComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Gérer le dash en cours
    if (bIsDashing)
    {
        UpdateDash(DeltaTime);
    }

    // Gérer le cooldown du dash
    if (!bCanDash)
    {
        CurrentCooldownTime += DeltaTime;
        if (CurrentCooldownTime >= DashCooldown)
        {
            bCanDash = true;
            CurrentCooldownTime = 0.0f;
        }
    }
}

void USR_DashComponent::Dash()
{   
    // Vérifier si le dash est possible
    if (!bCanDash || !CharacterMovement || !OwnerCharacter) return;

    // Désactiver temporairement le dash
    bCanDash = false;
    bIsDashing = true;

    // Normaliser la direction
    DashDirection.Normalize();

    // Stocker la position de départ
    DashStartLocation = OwnerCharacter->GetActorLocation();

    // Réinitialiser le temps écoulé
    CurrentDashTime = 0.0f;

    // Désactiver la gravité pendant le dash
    CharacterMovement->GravityScale = 0.0f;
    CharacterMovement->BrakingDecelerationFalling = 0.0f;

    // Optionnel : Ajouter un effet visuel/sonore ici
    UE_LOG(LogTemp, Warning, TEXT("Dash Started!"));
}

void USR_DashComponent::UpdateDash(float DeltaTime)
{
    if (!bIsDashing || !OwnerCharacter) return;

    // Incrémenter le temps de dash
    CurrentDashTime += DeltaTime;

    // Calculer la progression du dash
    float Alpha = FMath::Clamp(CurrentDashTime / DashDuration, 0.0f, 1.0f);

    // Calculer la nouvelle position
    FVector NewLocation = DashStartLocation + (DashDirection * DashDistance * (1.0f - FMath::Pow(1.0f - Alpha, 3)));
    
    // Déplacer le personnage
    OwnerCharacter->SetActorLocation(NewLocation, true);

    // Vérifier si le dash est terminé
    if (Alpha >= 1.0f)
    {
        EndDash();
    }
}

void USR_DashComponent::EndDash()
{
    if (!CharacterMovement) return;

    // Réactiver la gravité
    CharacterMovement->GravityScale = 1.0f;
    CharacterMovement->BrakingDecelerationFalling = 960.0f; // Valeur par défaut

    // Réinitialiser les flags
    bIsDashing = false;

    // Optionnel : Ajouter un effet visuel/sonore de fin de dash
    UE_LOG(LogTemp, Warning, TEXT("Dash Ended!"));
}