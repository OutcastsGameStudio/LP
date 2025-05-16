#include "SR_PanelControl.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "SR/Character/SR_Character.h"

ASR_PanelControl::ASR_PanelControl() {
  PrimaryActorTick.bCanEverTick = true;

  PanelMeshComponent =
      CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PanelMesh"));
  RootComponent = PanelMeshComponent;

  CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
  CollisionBox->SetupAttachment(RootComponent);

  CollisionBox->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
  CollisionBox->SetCollisionProfileName(TEXT("Trigger"));
  CollisionBox->SetGenerateOverlapEvents(true);

  CollisionBox->OnComponentBeginOverlap.AddDynamic(
      this, &ASR_PanelControl::OnOverlapBegin);
  CollisionBox->OnComponentEndOverlap.AddDynamic(
      this, &ASR_PanelControl::OnOverlapEnd);

  bIsActivated = false;
  bPlayerInRange = false;
  bIsBusy = false;
}

void ASR_PanelControl::BeginPlay() {
  Super::BeginPlay();

  if (!bIsActivated && InactiveMaterial) {
    PanelMeshComponent->SetMaterial(0, InactiveMaterial);
  } else if (bIsActivated && ActiveMaterial) {
    PanelMeshComponent->SetMaterial(0, ActiveMaterial);
  }
}

void ASR_PanelControl::Tick(float DeltaTime) {
  Super::Tick(DeltaTime);

  if (!bPlayerInRange) {
    WantActivatePanel();
  }

  if (bIsBusy && ArePlatformsIdle()) {
    if (!GetWorldTimerManager().IsTimerActive(CooldownTimerHandle)) {
      GetWorldTimerManager().SetTimer(CooldownTimerHandle, this,
                                      &ASR_PanelControl::OnCooldownEnd,
                                      CooldownTime, false);
    }
  }
}

void ASR_PanelControl::TogglePanel() {
  if (bIsBusy) {
    return;
  }

  bIsActivated = !bIsActivated;
  bIsBusy = true;

  if (bIsActivated && ActiveMaterial) {
    PanelMeshComponent->SetMaterial(0, ActiveMaterial);
  } else if (!bIsActivated && InactiveMaterial) {
    PanelMeshComponent->SetMaterial(0, InactiveMaterial);
  }

  for (ASR_BridgePlatform *Platform : ControlledPlatforms) {
    if (Platform) {
      Platform->ActivateMovement(bIsActivated);
    }
  }
}

bool ASR_PanelControl::IsPanelActive() const { return bIsActivated; }

void ASR_PanelControl::OnOverlapBegin(UPrimitiveComponent *OverlappedComp,
                                      AActor *OtherActor,
                                      UPrimitiveComponent *OtherComp,
                                      int32 OtherBodyIndex, bool bFromSweep,
                                      const FHitResult &SweepResult) {
  Character = Cast<ACharacter>(OtherActor);
  if (Character && Character->IsPlayerControlled()) {
    bPlayerInRange = true;
    OwnerCharacter = Cast<ASR_Character>(Character);
  }
}

void ASR_PanelControl::OnOverlapEnd(UPrimitiveComponent *OverlappedComp,
                                    AActor *OtherActor,
                                    UPrimitiveComponent *OtherComp,
                                    int32 OtherBodyIndex) {
  ACharacter *ExitingCharacter = Cast<ACharacter>(OtherActor);
  if (ExitingCharacter && ExitingCharacter == Character) {
    bPlayerInRange = false;
    Character = nullptr;
    OwnerCharacter = nullptr;
  }
}

void ASR_PanelControl::WantActivatePanel() {
  APlayerController *PlayerController =
      UGameplayStatics::GetPlayerController(GetWorld(), 0);
  if (PlayerController) {
    APawn *PlayerPawn = PlayerController->GetPawn();
    if (PlayerPawn) {
      float Distance =
          FVector::Dist(PlayerPawn->GetActorLocation(), GetActorLocation());
      if (Distance <= ActivationDistance) {
        bPlayerInRange = true;
        Character = Cast<ACharacter>(PlayerPawn);
        OwnerCharacter = Cast<ASR_Character>(Character);
      } else {
        bPlayerInRange = false;
        Character = nullptr;
        OwnerCharacter = nullptr;
      }
    }
  }
}

void ASR_PanelControl::TryActivatePanel() {
  if (bIsBusy || GetWorldTimerManager().IsTimerActive(CooldownTimerHandle)) {
    return;
  }

  if (bPlayerInRange && Character && Character->IsPlayerControlled()) {
    TogglePanel();
  }
}

void ASR_PanelControl::OnCooldownEnd() { bIsBusy = false; }

bool ASR_PanelControl::ArePlatformsIdle() const {
  for (ASR_BridgePlatform *Platform : ControlledPlatforms) {
    if (Platform && Platform->IsMoving()) {
      return false;
    }
  }

  return true;
}

void ASR_PanelControl::ResetPanel() { bIsActivated = false; }
