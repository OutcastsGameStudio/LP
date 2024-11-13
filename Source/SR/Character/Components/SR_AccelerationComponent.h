// Component responsible for handling the acceleration of the character

#pragma once

#include "SR_AccelerationComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SR_API USR_AccelerationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USR_AccelerationComponent();	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// acceleration value
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Acceleration")
	float Acceleration = 1000.f;

	// deceleration value
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Acceleration")
	float Deceleration = 1000.f;

	// max speed value
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Acceleration")
	float MaxSpeed = 1000.f;

	// current speed value
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Acceleration")
	float CurrentSpeed = 0.f;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	// function to accelerate the character over time which will return the new speed
	UFUNCTION(BlueprintCallable, Category = "Acceleration")
	float Accelerate(float Speed, float DeltaTime); // function to accelerate the character over time	
	
	// function to decelerate the character over time
	UFUNCTION(BlueprintCallable, Category = "Acceleration")
	float Decelerate(float Speed, float DeltaTime); // function to decelerate the character over time

	// function to increase the max speed of the character
	UFUNCTION(BlueprintCallable, Category = "Acceleration")
	void IncreaseMaxSpeed(float Value); // function to increase the max speed of the character

	// function to reset the max speed of the character to the default value
	UFUNCTION(BlueprintCallable, Category = "Acceleration")
	void ResetMaxSpeed(); // function to reset the max speed of the character to the default value

	// function to get the current speed of the character
	UFUNCTION(BlueprintCallable, Category = "Acceleration")
	float GetCurrentSpeed(); // function to get the current speed of the character

	// function to set the current speed of the character
	UFUNCTION(BlueprintCallable, Category = "Acceleration")
	void SetCurrentSpeed(float Value); // function to set the current speed of the character
};
