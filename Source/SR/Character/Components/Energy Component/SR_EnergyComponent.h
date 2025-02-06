#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SphereComponent.h"
#include "SR_EnergyComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SR_API USR_EnergyComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USR_EnergyComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							 FActorComponentTickFunction* ThisTickFunction) override;

	// Propriétés exposées à l'éditeur
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy System")
	int32 MaxEnergySlots = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy System")
	float DetectionRadius = 200.0f;

	// Variables pour le système d'énergie
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Energy System")
	int32 CurrentEnergySlots = 0;

	// Fonctions pour la gestion de l'énergie
	UFUNCTION(BlueprintCallable, Category = "Energy System")
	bool UseEnergySlot();

	UFUNCTION(BlueprintCallable, Category = "Energy System")
	void AddEnergySlot();

	UFUNCTION(BlueprintCallable, Category = "Energy System")
	float GetEnergyPercentage() const;

	// Fonction pour détecter les objets interactifs
	void DetectInteractableObjects();

private:
	// Référence au propriétaire du composant
	AActor* OwningActor;

	// Fonction pour vérifier si un objet est dans le rayon de détection
	bool IsInRange(AActor* OtherActor) const;
};