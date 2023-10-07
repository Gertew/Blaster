// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "BlasterGameMode.generated.h"

class ABlasterCharacter;
class ABlasterPlayerController;
class UEnvQuery;
class AWeapon;
class ABlasterPlayerState;

namespace MatchState
{
	extern BLASTER_API const FName Cooldown; // Match Duration has been reached
}

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ABlasterGameMode();
	virtual void Tick(float DeltaTime) override;
	virtual void PlayerEliminated(ABlasterCharacter* VictimCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController);
	virtual void RequestRespawn(ACharacter* VictimCharacter, AController* VictimController);
	void PlayerLeftGame(ABlasterPlayerState* PlayerLeaving);

	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage);

	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.0f;
	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 120.0f;
	UPROPERTY(EditDefaultsOnly)
	float CooldownTime = 10.0f;
	float LevelStartingTime = 0.0f;

	bool bTeamsMatch = false;

protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UEnvQuery> RandomSpawnEQS;

	void HandleEQSFinish(TSharedPtr<FEnvQueryResult> Result);

	float CountdownTime = 0.0f;

public:
	FORCEINLINE float GetCountdownTime() const { return CountdownTime; }
	FORCEINLINE bool IsTeamsMatch() const { return bTeamsMatch; }

};
