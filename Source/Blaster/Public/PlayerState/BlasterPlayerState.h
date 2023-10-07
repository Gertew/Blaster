// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BlasterTypes/Team.h"
#include "BlasterPlayerState.generated.h"

class ABlasterPlayerController;
class ABlasterCharacter;

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerState : public APlayerState
{
	GENERATED_BODY()
	

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void UpdatePing(float InPing) override;
	void AddToScore(float ScoreAmount);
	void AddToDefeats(int32 DefeatsAmount);

	virtual void OnRep_Score() override;
	UFUNCTION()
	virtual void OnRep_Defeats();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastDeathMessage(const FString& ElimmedBy);

protected:

private:
	UPROPERTY()
	TObjectPtr<ABlasterPlayerController> Controller;
	UPROPERTY()
	TObjectPtr<ABlasterCharacter> Character;

	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats;

	UPROPERTY(ReplicatedUsing = OnRep_Team)
	ETeam Team = ETeam::ET_NoTeam;

	UFUNCTION()
	void OnRep_Team();

public:
	FORCEINLINE ETeam GetTeam() const { return Team; }
	void SetTeam(ETeam NewTeam);

};
