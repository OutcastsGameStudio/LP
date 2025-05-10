// Fill out your copyright notice in the Description page of Project Settings.


#include "SR_DebugComponent.h"

#include "SR_DebugData.h"
#include "SR/Character/SR_Character.h"


// Sets default values for this component's properties
USR_DebugComponent::USR_DebugComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	CharacterOwner = Cast<ASR_Character>(GetOwner());
}


// Called when the game starts
void USR_DebugComponent::BeginPlay()
{
	Super::BeginPlay();
	CharacterMovementComponent = Cast<USR_CharacterMovementComponent>(CharacterOwner->GetCharacterMovement());
	//LoadDataTable();
	// ...
	
}


// Called every frame
void USR_DebugComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USR_DebugComponent::SetCharacterLedgeGrabReachDistance(float newReachDistance)
{
	CharacterOwner->SetCharacterLedgeGrabReachDistance(newReachDistance);
	UpdateTable("CharacterLedgeGrabReachDistance", newReachDistance);
}

float USR_DebugComponent::GetCharacterLedgeGrabReachDistance()
{
	return CharacterOwner->GetCharacterLedgeGrabReachDistance();
}

void USR_DebugComponent::SetLedgeGrabHeight(float newLedgeGrabHeight)
{
	CharacterOwner->SetLedgeGrabHeight(newLedgeGrabHeight);
	UpdateTable("LedgeGrabHeight", newLedgeGrabHeight);
}

float USR_DebugComponent::GetLedgeGrabHeight()
{
	return CharacterOwner->GetLedgeGrabHeight();
}

void USR_DebugComponent::SetClimbUpSpeed(float newClimbUpSpeed)
{
	CharacterOwner->SetClimbUpSpeed(newClimbUpSpeed);
	UpdateTable("ClimbUpSpeed", newClimbUpSpeed);
}

float USR_DebugComponent::GetClimbUpSpeed()
{
	return CharacterOwner->GetClimbUpSpeed();
}

float USR_DebugComponent::GetWallJumpSpeed() 
{
	// if(CharacterMovementComponent == nullptr) return 0.f;
	// return CharacterMovementComponent->GetWallJumpSpeed();
	return 0;
}

void USR_DebugComponent::SetWallJumpSpeed(float NewWallJumpSpeed)
{
	// if(CharacterMovementComponent == nullptr) return;
	// CharacterMovementComponent->SetWallJumpSpeed(NewWallJumpSpeed);
	// UpdateTable("WallJumpSpeed", NewWallJumpSpeed);
}

float USR_DebugComponent::GetMaxAngleWallRun() 
{
	// if(CharacterMovementComponent == nullptr) return 0.f;
	// return CharacterMovementComponent->GetMaxAngleWallRun();
	return 0;
}

void USR_DebugComponent::SetMaxAngleWallRun(float NewMaxAngleWallRun)
{
	// if(CharacterMovementComponent == nullptr) return;
	// CharacterMovementComponent->SetMaxAngleWallRun(NewMaxAngleWallRun);
	// UpdateTable("MaxAngleWallRun", NewMaxAngleWallRun);
}

float USR_DebugComponent::GetMaxAngleBeforeStop() 
{
	// if(CharacterMovementComponent == nullptr) return 0.f;
	// return CharacterMovementComponent->GetMaxAngleBeforeStop();
	return 0;
}

void USR_DebugComponent::SetMaxAngleBeforeStop(float NewMaxAngleBeforeStop)
{
	// if(CharacterMovementComponent == nullptr) return;
	// CharacterMovementComponent->SetMaxAngleBeforeStop(NewMaxAngleBeforeStop);
	// UpdateTable("MaxAngleBeforeStop", NewMaxAngleBeforeStop);
}

float USR_DebugComponent::GetWallRunFallingAcceleration() 
{
	// if(CharacterMovementComponent == nullptr) return 0.f;
	// return CharacterMovementComponent->GetWallRunFallingAcceleration();
	return 0;
}

void USR_DebugComponent::SetWallRunFallingAcceleration(float NewWallRunFallingAcceleration)
{
	// if(CharacterMovementComponent == nullptr) return;
	// CharacterMovementComponent->SetWallRunFallingAcceleration(NewWallRunFallingAcceleration);
	// UpdateTable("WallRunFallingAcceleration", NewWallRunFallingAcceleration);
}

void USR_DebugComponent::InitDataTable()
{
	for(auto prop: DebugProps)
	{
		if(prop == "CharacterLedgeGrabReachDistance")
		{
			FSR_DebugData data;
			data.Value = CharacterOwner->GetCharacterLedgeGrabReachDistance();
			DebugDataTable->AddRow(prop, data);
		} else if(prop ==  "LedgeGrabHeight")
		{
			FSR_DebugData data;
			data.Value = CharacterOwner->GetLedgeGrabHeight();
			DebugDataTable->AddRow(prop, data);
		} else if (prop == "ClimbUpSpeed")
		{
			FSR_DebugData data;
			data.Value = CharacterOwner->GetClimbUpSpeed();
			DebugDataTable->AddRow(prop, data);
		} else if (prop == "WallJumpSpeed")
		{
			FSR_DebugData data;
			data.Value = GetWallJumpSpeed();
			DebugDataTable->AddRow(prop, data);
		} else if (prop == "MaxAngleWallRun")
		{
			FSR_DebugData data;
			data.Value = GetMaxAngleWallRun();
			DebugDataTable->AddRow(prop, data);
		} else if (prop == "MaxAngleBeforeStop")
		{
			FSR_DebugData data;
			data.Value = GetMaxAngleBeforeStop();
			DebugDataTable->AddRow(prop, data);
		} else if (prop == "WallRunFallingAcceleration")
		{
			FSR_DebugData data;
			data.Value = GetWallRunFallingAcceleration();
			DebugDataTable->AddRow(prop, data);
		}
	}
}

void USR_DebugComponent::SetValuesFromDataTable()
{
	// if (DebugDataTable->GetRowNames().Num() == 0)
	// {
	// 	return;
	// }
	//
	// for (const auto& prop : DebugProps)
	// {
	// 	if (const FSR_DebugData* RowData = DebugDataTable->FindRow<FSR_DebugData>(FName(prop), TEXT("")))
	// 	{
	// 		if (prop == "CharacterLedgeGrabReachDistance")
	// 		{
	// 			CharacterOwner->SetCharacterLedgeGrabReachDistance(RowData->Value);
	// 		}
	// 		else if (prop == "LedgeGrabHeight")
	// 		{
	// 			CharacterOwner->SetLedgeGrabHeight(RowData->Value);
	// 		}
	// 		else if (prop == "ClimbUpSpeed")
	// 		{
	// 			CharacterOwner->SetClimbUpSpeed(RowData->Value);
	// 		}
	// 		else if (prop == "WallJumpSpeed" && CharacterMovementComponent)
	// 		{
	// 			CharacterMovementComponent->SetWallJumpSpeed(RowData->Value);
	// 		}
	// 		else if (prop == "MaxAngleWallRun" && CharacterMovementComponent)
	// 		{
	// 			CharacterMovementComponent->SetMaxAngleWallRun(RowData->Value);
	// 		}
	// 		else if (prop == "MaxAngleBeforeStop" && CharacterMovementComponent)
	// 		{
	// 			CharacterMovementComponent->SetMaxAngleBeforeStop(RowData->Value);
	// 		}
	// 		else if (prop == "WallRunFallingAcceleration" && CharacterMovementComponent)
	// 		{
	// 			CharacterMovementComponent->SetWallRunFallingAcceleration(RowData->Value);
	// 		}
	// 	}
	// }
}

void USR_DebugComponent::UpdateTable(FName RowName, float NewValue)
{
	// Trouver la ligne existante
	if (FSR_DebugData* ExistingRow = DebugDataTable->FindRow<FSR_DebugData>(RowName, TEXT("")))
	{
		// Modifier la valeur
		ExistingRow->Value = NewValue;
	}
	else 
	{
		// Si la ligne n'existe pas, l'ajouter
		FSR_DebugData NewData;
		NewData.Value = NewValue;
		DebugDataTable->AddRow(RowName, NewData);
	}
}

void USR_DebugComponent::LoadDataTable()
{
	if(DebugDataTable->GetRowNames().Num() == 0)
	{
		InitDataTable();
	} else
	{
		SetValuesFromDataTable();
	}
}

