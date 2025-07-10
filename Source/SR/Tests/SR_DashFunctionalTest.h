#pragma once

#include "CoreMinimal.h"
#include "FunctionalTest.h"
#include "SR/Character/SR_Character.h"
#include "SR/Character/Components/Dash/SR_DashComponent.h"
#include "SR_DashFunctionalTest.generated.h"

UENUM(BlueprintType)
enum class EDashTestPhase : uint8
{
    Setup,
    TestGroundDash,
    TestAirDash,
    TestGroundCooldown,
    TestAirCooldown,
    TestWallRunReset,
    TestEvents,
    Complete
};

/**
 * Functional Test for the Dash Component
 * Tests all dash functionalities including cooldowns, state transitions, and events
 */
UCLASS(BlueprintType)
class SR_API ASR_DashFunctionalTest : public AFunctionalTest
{
    GENERATED_BODY()

public:
    ASR_DashFunctionalTest();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // Test configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test Settings")
    TSubclassOf<ASR_Character> CharacterClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test Settings")
    float TestTimeout = 30.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test Settings")
    float GroundDashExpectedSpeed = 2000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test Settings")
    float AirDashExpectedSpeed = 2000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test Settings")
    float GroundCooldownExpected = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test Settings")
    float AirCooldownExpected = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test Settings")
    float TolerancePercentage = 0.1f; // 10% tolerance

private:
    // Test state
    UPROPERTY()
    ASR_Character* TestCharacter;

    UPROPERTY()
    USR_DashComponent* DashComponent;

    EDashTestPhase CurrentPhase;
    float PhaseStartTime;
    float TestStartTime;
    bool bDashStartedEventReceived;
    bool bDashEndedEventReceived;
    FVector InitialPosition;
    FVector DashStartPosition;

    // Test methods
    void SetupTest();
    void RunCurrentPhase();
    void AdvanceToNextPhase();
    void CompleteTest(bool bSuccess, const FString& Message);

    // Phase-specific test methods
    void TestGroundDash();
    void TestAirDash();
    void TestGroundCooldown();
    void TestAirCooldown();
    void TestWallRunReset();
    void TestEvents();

    // Utility methods
    void SpawnTestCharacter();
    void SetCharacterOnGround();
    void SetCharacterInAir();
    void SimulateWallRun();
    bool IsWithinTolerance(float Expected, float Actual) const;
    float GetCurrentPhaseTime() const;

    // Event handlers
    UFUNCTION()
    void OnDashStarted();

    UFUNCTION()
    void OnDashEnded();

    // Validation methods
    bool ValidateDashSpeed(float ExpectedSpeed);
    bool ValidateCooldownTime(float ExpectedCooldown, bool bIsAirDash);
    bool ValidatePosition(const FVector& StartPos, float MinDistance);
    bool ValidateCharacterState();
};