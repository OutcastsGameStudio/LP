// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SR_DashComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SR_API USR_DashComponent : public UActorComponent
{
	GENERATED_BODY()

public:    
	// Constructeur
	USR_DashComponent();

	// Fonction principale de dash
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void Dash();

protected:
	// Appelée au début du jeu
	virtual void BeginPlay() override;

public:    
	// Appelée à chaque frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, 
							   FActorComponentTickFunction* ThisTickFunction) override;

	// Direction du dash
	FVector DashDirection;

protected:
	// Composant de mouvement du personnage
	UPROPERTY()
	UCharacterMovementComponent* CharacterMovement;

	// Référence au personnage propriétaire
	UPROPERTY()
	class ACharacter* OwnerCharacter;

	// Paramètres configurables du dash
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash Settings")
	float DashDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash Settings")
	float DashSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash Settings")
	float DashDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash Settings")
	float DashCooldown;

	

private:
	// Mettre à jour le dash
	void UpdateDash(float DeltaTime);

	// Terminer le dash
	void EndDash();

	// Position de départ du dash
	FVector DashStartLocation;

	// Temps de dash actuel
	float CurrentDashTime;

	// Temps de cooldown actuel
	float CurrentCooldownTime;

	// Flags de contrôle
	bool bIsDashing;
	bool bCanDash;
};
