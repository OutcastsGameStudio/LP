// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "SR_DebugData.generated.h"

USTRUCT(BlueprintType)
struct SR_API FSR_DebugData : public FTableRowBase
{
	GENERATED_BODY()

	/** Id of the row */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Example)
	float Value;
};
