#include "SR_PanelControl.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"


ASR_PanelControl::ASR_PanelControl()
{
	PrimaryActorTick.bCanEverTick = true;

	PanelMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PanelMesh"));
	RootComponent = PanelMeshComponent;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(RootComponent);
    
	CollisionBox->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	CollisionBox->SetCollisionProfileName(TEXT("Trigger"));
	CollisionBox->SetGenerateOverlapEvents(true);
    
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ASR_PanelControl::OnOverlapBegin);
    
	bIsActivated = false;
}

void ASR_PanelControl::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASR_PanelControl::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASR_PanelControl::TogglePanel()
{
	bIsActivated = !bIsActivated;

	if (bIsActivated && ActiveMaterial)
	{
		PanelMeshComponent->SetMaterial(0, ActiveMaterial);
	}
	else if (!bIsActivated && InactiveMaterial)
	{
		PanelMeshComponent->SetMaterial(0, InactiveMaterial);
	}

	for (ASR_BridgePlatform* Platform : ControlledPlatforms)
	{
		if (Platform)
		{
			Platform->ActivateMovement(bIsActivated);
		}
	}
}

bool ASR_PanelControl::IsPanelActive() const
{
	return bIsActivated;
}

void ASR_PanelControl::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
									 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
									 bool bFromSweep, const FHitResult& SweepResult)
{
	Character = Cast<ACharacter>(OtherActor);
	if (Character && Character->IsPlayerControlled())
	{
		TogglePanel();
	}
}