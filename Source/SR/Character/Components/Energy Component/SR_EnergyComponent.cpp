#include "SR_EnergyComponent.h"
#include "GameFramework/Character.h"

USR_EnergyComponent::USR_EnergyComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    CurrentEnergySlots = 0;
}

void USR_EnergyComponent::BeginPlay()
{
    Super::BeginPlay();
    OwningCharacter = Cast<ACharacter>(GetOwner());
}

bool USR_EnergyComponent::UseEnergySlot()
{
    return CurrentEnergySlots > 0 ? (--CurrentEnergySlots, true) : false;
}

void USR_EnergyComponent::AddEnergySlot()
{
    CurrentEnergySlots = FMath::Min(CurrentEnergySlots + 1, MaxEnergySlots);
}

float USR_EnergyComponent::GetEnergySlotRemaining() const
{
    return MaxEnergySlots - CurrentEnergySlots;
}