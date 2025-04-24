// Fill out your copyright notice in the Description page of Project Settings.


#include "SR_MotionController.h"

#include "SR/Character/SR_Character.h"


// Sets default values for this component's properties
USR_MotionController::USR_MotionController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.

	// ...
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void USR_MotionController::BeginPlay()
{
	Super::BeginPlay();

	ASR_Character* OwnerCharacter = Cast<ASR_Character>(GetOwner());
	if (OwnerCharacter)
	{
		m_OwnerCharacter = OwnerCharacter;
		m_CharacterMovementComponent = Cast<USR_CharacterMovementComponent>(OwnerCharacter->GetCharacterMovement());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Owner character is not of type ASR_Character"));
	}

	// ...
	
}


// Called every frame
void USR_MotionController::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

int32 USR_MotionController::ApplyRootMotion(const FRootMotionRequest& Request)
{
	if (!m_CharacterMovementComponent || Request.Direction.IsZero() || Request.Strength <= 0.0f)
    {
        return -1;
    }

    // Create root motion source
    TSharedPtr<FRootMotionSource_ConstantForce> ConstantForce = MakeShared<FRootMotionSource_ConstantForce>();
    
    // Setup the root motion source
    ConstantForce->InstanceName = Request.MovementName;
    ConstantForce->AccumulateMode = Request.bIsAdditive ? ERootMotionAccumulateMode::Additive : ERootMotionAccumulateMode::Override;
    ConstantForce->Priority = static_cast<int32>(Request.Priority);
    ConstantForce->Force = Request.Direction.GetSafeNormal() * Request.Strength;
    ConstantForce->Duration = Request.Duration;
    ConstantForce->StrengthOverTime = Request.StrengthOverTime;
    
    // Setup the finish velocity parameters
    ConstantForce->FinishVelocityParams.Mode = Request.VelocityOnFinish;
    ConstantForce->FinishVelocityParams.SetVelocity = Request.SetVelocityOnFinish;
    ConstantForce->FinishVelocityParams.ClampVelocity = Request.ClampVelocityOnFinish;
    
    // Handle gravity
    if (Request.bEnableGravity)
    {
        ConstantForce->Settings.SetFlag(ERootMotionSourceSettingsFlags::IgnoreZAccumulate);
    }
    
    // Apply the root motion source
    int32 RootMotionID = m_CharacterMovementComponent->ApplyRootMotionSource(ConstantForce);
    
    if (RootMotionID != -1)
    {
        // Save the active root motion
        FActiveRootMotion NewActiveMotion;
        NewActiveMotion.ID = RootMotionID;
        NewActiveMotion.Name = Request.MovementName;
        NewActiveMotion.StartTime = GetWorld()->GetTimeSeconds();
        NewActiveMotion.Duration = Request.Duration;
        ActiveRootMotions.Add(NewActiveMotion);
        
        // Set up a timer to remove the root motion after the duration
        if (Request.Duration > 0.0f)
        {
            FTimerHandle TimerHandle;
            FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(
                this, &USR_MotionController::OnRootMotionFinished, RootMotionID, Request.MovementName);
                
            GetWorld()->GetTimerManager().SetTimer(
                TimerHandle, TimerDelegate, Request.Duration, false);
                
            RootMotionTimers.Add(RootMotionID, TimerHandle);
        }
    }
    
    return RootMotionID;
}

bool USR_MotionController::CancelRootMotion(int32 RootMotionID)
{
	if (!m_CharacterMovementComponent || RootMotionID == -1)
	{
		return false;
	}
    
	// Trouver le mouvement actif
	FActiveRootMotion* ActiveMotion = nullptr;
	int32 MotionIndex = INDEX_NONE;
    
	for (int32 i = 0; i < ActiveRootMotions.Num(); ++i)
	{
		if (ActiveRootMotions[i].ID == RootMotionID)
		{
			ActiveMotion = &ActiveRootMotions[i];
			MotionIndex = i;
			break;
		}
	}
    
	if (ActiveMotion)
	{
		// Supprimer le root motion
		// m_CharacterMovementComponent->RemoveRootMotionSourceByID(RootMotionID);
        
		// Nettoyer le timer
		FTimerHandle* TimerHandle = RootMotionTimers.Find(RootMotionID);
		if (TimerHandle)
		{
			GetWorld()->GetTimerManager().ClearTimer(*TimerHandle);
			RootMotionTimers.Remove(RootMotionID);
		}
        
		// Appeler le callback avec bInterrupted = true
		OnRootMotionCompleted.Broadcast(RootMotionID, true);
        
		// Retirer de la liste des mouvements actifs
		ActiveRootMotions.RemoveAt(MotionIndex);
        
		return true;
	}
    
	return false;
}

void USR_MotionController::CancelAllRootMotions()
{
	// Nettoyer tous les timers
	for (auto& Pair : RootMotionTimers)
	{
		GetWorld()->GetTimerManager().ClearTimer(Pair.Value);
	}
	RootMotionTimers.Empty();
    
	// Broadcast pour tous les mouvements actifs
	for (const FActiveRootMotion& Motion : ActiveRootMotions)
	{
		OnRootMotionCompleted.Broadcast(Motion.ID, true);
		// m_CharacterMovementComponent->RemoveRootMotionSourceByID(Motion.ID);
	}
    
	ActiveRootMotions.Empty();
}

bool USR_MotionController::IsRootMotionActive(FName MovementName)
{
	for (const FActiveRootMotion& Motion : ActiveRootMotions)
	{
		if (Motion.Name == MovementName)
		{
			return true;
		}
	}
    
	return false;
}

void USR_MotionController::OnRootMotionFinished(int32 RootMotionID, FName MovementName)
{
	// Trouver et supprimer le mouvement actif
	int32 MotionIndex = INDEX_NONE;
    
	for (int32 i = 0; i < ActiveRootMotions.Num(); ++i)
	{
		if (ActiveRootMotions[i].ID == RootMotionID)
		{
			MotionIndex = i;
			break;
		}
	}
    
	if (MotionIndex != INDEX_NONE)
	{
		ActiveRootMotions.RemoveAt(MotionIndex);
	}
    
	// Nettoyer le timer
	RootMotionTimers.Remove(RootMotionID);

	// Notifier de la fin du mouvement (terminé normalement)
	OnRootMotionCompleted.Broadcast(RootMotionID, false);
}



