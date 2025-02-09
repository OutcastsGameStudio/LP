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

	// Properties exposed to the editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy System")
	int32 MaxEnergySlots = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy System")
	float DetectionRadius = 200.0f;

	// Variables for the energy system
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Energy System")
	int32 CurrentEnergySlots = 0;

	// Functions for energy management
	UFUNCTION(BlueprintCallable, Category = "Energy System")
	bool UseEnergySlot();

	UFUNCTION(BlueprintCallable, Category = "Energy System")
	void AddEnergySlot();

	UFUNCTION(BlueprintCallable, Category = "Energy System")
	int32 GetEnergySlotRemaining() const;

	// Function to check if energy is empty
	UFUNCTION(BlueprintCallable, Category = "Energy System")
	bool IsEnergyEmpty() const;

	// Function to check if energy is full
	UFUNCTION(BlueprintCallable, Category = "Energy System")
	bool IsEnergyFull() const;

	// Function to detect interactive objects
	void DetectInteractableObjects();

	// Function to check if an object is interactive
	UFUNCTION(BlueprintCallable, Category = "Energy System")
	bool IsInteractable(AActor* OtherActor) const;

private:
	// Reference to the component owner
	AActor* OwningActor;

	// Function to check if an object is within the detection radius
	bool IsInRange(AActor* OtherActor) const;
};