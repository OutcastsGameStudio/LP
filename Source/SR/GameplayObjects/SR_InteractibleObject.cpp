#include "SR_InteractibleObject.h"
#include "Components/SphereComponent.h"
#include "SR/Character/Components/Energy Component/SR_EnergyComponent.h"


ASR_InteractibleObject::ASR_InteractibleObject()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
	RootComponent = TriggerSphere;
	TriggerSphere->SetSphereRadius(InteractionRadius);
	TriggerSphere->SetCollisionProfileName(TEXT("Trigger"));
}

void ASR_InteractibleObject::BeginPlay()
{
	Super::BeginPlay();
    
	TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &ASR_InteractibleObject::OnOverlapBegin);
	TriggerSphere->OnComponentEndOverlap.AddDynamic(this, &ASR_InteractibleObject::OnOverlapEnd);
}

void ASR_InteractibleObject::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (USR_EnergyComponent* EnergyComp = OtherActor->FindComponentByClass<USR_EnergyComponent>())
	{
		if (Tags.Contains("EnergyPickup"))
		{
			EnergyComp->AddEnergySlot();
			Destroy();
		}
		else
		{
			OnInteractionStart();
		}
	}
}

void ASR_InteractibleObject::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}