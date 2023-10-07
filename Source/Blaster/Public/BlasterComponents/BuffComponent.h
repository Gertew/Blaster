// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"

class ABlasterCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBuffComponent();
	friend class ABlasterCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma region Initialization
	void SetInitialSpeeds(float BaseSpeed, float CrouchSpeed);
	void SetInitialJumpVelocity(float BaseVelocity);
#pragma endregion Initializationg

#pragma region Apply Effects
	void Heal(float HealAmount, float HealingTime);
	void ReplenishShield(float ReplenishAmount, float ReplenishTime);
	void BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime);
	void BuffJump(float BuffJumpVelocity, float BuffTime);
#pragma endregion Apply Effects

protected:
	virtual void BeginPlay() override;

#pragma region Over Time Effects
	void HealRampUp(float DeltaTime);
	void ShieldRampUp(float DeltaTime);
#pragma endregion Over Time Effects

private:
#pragma region Buff Internals
	UPROPERTY()
	TObjectPtr<ABlasterCharacter> Character;
#pragma endregion Buff Internals

#pragma region Healing
	bool bHealing = false;
	float HealingRate = 0.0f;
	float AmountToHeal = 0.0f;
#pragma endregion Healing

#pragma region Shielding
	bool bReplenishingShield = false;
	float ShieldReplenishRate = 0.0f;
	float ShieldReplenishAmount = 0.0f;
#pragma endregion Shielding

#pragma region Speed
	FTimerHandle SpeedBuffTimer;
	void ResetSpeeds();
	float InitialBaseSpeed;
	float InitialCrouchSpeed;
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SpeedBuff(float BaseSpeed, float CrouchSpeed);
#pragma endregion Speed

#pragma region Jump
	FTimerHandle JumpBuffTimer;
	void ResetJump();
	float InitialJumpVelocity;
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_JumpBuff(float BuffJumpVelocity);
#pragma endregion Jump
		
};
