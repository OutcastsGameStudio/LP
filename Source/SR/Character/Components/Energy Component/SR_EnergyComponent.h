#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Energy")
	float MaxEnergySlots = 5;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Energy")
	float CurrentEnergySlots = 0;

	UFUNCTION(BlueprintCallable, Category = "Energy")
	bool UseEnergySlot();

	UFUNCTION(BlueprintCallable, Category = "Energy")
	void AddEnergySlot();

	UFUNCTION(BlueprintCallable, Category = "Energy")
	float GetEnergySlotRemaining() const;

	UFUNCTION(BlueprintPure, Category = "Energy")
	bool IsEnergyEmpty() const { return CurrentEnergySlots == 0; }

	UFUNCTION(BlueprintPure, Category = "Energy")
	bool IsEnergyFull() const { return CurrentEnergySlots == MaxEnergySlots; }

private:
	class ACharacter* OwningCharacter;
};