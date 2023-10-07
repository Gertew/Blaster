// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

#pragma region Forward Declarations
class ABlasterHUD;
class UCharacterOverlay;
class ABlasterGameMode;
class UInputAction;
class UUserWidget;
class UOptionsMenu;
class ABlasterPlayerState;
class ABlasterGameState;
#pragma endregion Forward Declarations

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighPingDelegate, bool, bPingTooHigh);

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;

#pragma region HUD

#pragma region Player Attributes
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDShield(float Shield, float MaxShield);
#pragma endregion Player Attributes

#pragma region Score
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
#pragma endregion Score

#pragma region Weapon/Ammo
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDGrenades(int32 Grenades);
	void SetHUDWeaponIcon(UTexture2D* Icon);
#pragma endregion Weapon/Ammo

#pragma region Match State
	void SetHUDMatchCountdown(float CountdownTime);
	void SetHUDAnnouncementCountdown(float CountdownTime);
	void SetHUDLatency(float Latency);
#pragma endregion Match State

#pragma region Elimination
	void UpdateHUDElim(const FString& AttackingPlayer);
	void HideHUDElim();
#pragma endregion Elimination

#pragma region Team
	void HideHUDTeamScores();
	void InitHUDTeamScores();
	void SetHUDRedTeamScore(int32 Score);
	void SetHUDBlueTeamScore(int32 Score);
#pragma endregion Team

#pragma endregion HUD

#pragma region Server
	// Synced with Server World Clock
	virtual float GetServerTime();
	// Sync with Server as Soon as Possible
	virtual void ReceivedPlayer() override;
	void OnMatchStateSet(FName State, bool bTeamsMatch = false);
	void HandleCooldown();

	float SingleTripTime = 0;
	FHighPingDelegate HighPingDelegate;
#pragma endregion Server

#pragma region Elimination
	void BroadcastElimination(APlayerState* Attacker, APlayerState* Victim);
#pragma endregion Elimination

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

#pragma region HUD Setup
	void PollInit();
	void SetHUDTime();
	void HandleMatchHasStarted(bool bTeamsMatch = false);
#pragma endregion HUD Setup

#pragma region Server Sync
	// Client -> Server - Client requests the current Server Time
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float ClientRequestTime);
	// Server -> Client - Server reports to Client the current Server Time in response to the ServerRequest 
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float ClientRequestTime, float ServerReceievedClientRequestTime);

	// Difference between Client and Server Times
	float ClientServerDelta = 0.0f;

	void CheckTimeSync(float DeltaTime);
	UPROPERTY(EditDefaultsOnly, Category = Time)
	float TimeSyncFrequency = 5.0f;

	float TimeSyncRunningTime = 0.0f;

	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();

	UFUNCTION(Client, Reliable)
	void ClientJoinMidgame(FName StateOfMatch, float TimeOfWarmup, float TimeOfMatch, float TimeOfCooldown, float TimeOfStart, bool bIsTeamsMatch);

#pragma endregion Server Sync

#pragma region Latency Ping
	void HighPingWarning();
	void StopHighPingWarning();
	void CheckPing(float DeltaTime);
#pragma endregion Latency Ping

#pragma region Teams
	UPROPERTY(ReplicatedUsing = OnRep_ShowTeamScores)
	bool bShowTeamScores = false;

	UFUNCTION()
	void OnRep_ShowTeamScores();
#pragma endregion Teams

#pragma region End of Match Info
	FText GetInfoText(const TArray<ABlasterPlayerState*>& Players);
	FText GetTeamsInfoText(ABlasterGameState* BlasterGameState);
#pragma endregion End of Match Info

#pragma region Elimination
	UFUNCTION(Client, Reliable)
	void ClientElimination(APlayerState* Attacker, APlayerState* Victim);
#pragma endregion Elimination

private:

#pragma region Options Menu
	void ShowOptionsMenu();

	UPROPERTY(EditDefaultsOnly, Category = HUD)
	TSubclassOf<UUserWidget> OptionsMenuClass;
	UPROPERTY()
	TObjectPtr<UOptionsMenu> OptionsMenu;
	UPROPERTY(EditDefaultsOnly, Category = Input)
	TObjectPtr<UInputAction> OptionsMenuAction;

	bool bOptionsMenuOpen;
#pragma endregion Options Menu

#pragma region Player Controller Internals
	UPROPERTY()
	TObjectPtr<ABlasterHUD> BlasterHUD;

	UPROPERTY()
	TObjectPtr<ABlasterGameMode> BlasterGameMode;

	UPROPERTY()
	TObjectPtr<UCharacterOverlay> CharacterOverlay;
#pragma endregion Player Controller Internals

#pragma region Match State
	float MatchTime = 0.0f;
	float WarmupTime = 0.0f;
	float CooldownTime = 0.0f;
	float LevelStartingTime = 0.0f;
	uint32 CountdownInt = 0;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();
#pragma endregion Match State

#pragma region Internal HUD Setup

	bool bInitializeHealth = false;
	float HUDHealth;
	float HUDMaxHealth;

	bool bInitializeShield = false;
	float HUDShield;
	float HUDMaxShield;

	bool bInitializeGrenades = false;
	int32 HUDGrenades;

	bool bInitializeScore = false;
	float HUDScore;

	bool bInitializeDefeats = false;
	int32 HUDDefeats;

	bool bInitializeCarriedAmmo = false;
	float HUDCarriedAmmo;

	bool bInitializeWeaponAmmo = false;
	float HUDWeaponAmmo;

	bool bInitializeWeaponIcon = false;
	TObjectPtr<UTexture2D> HUDWeaponIcon;

	bool bInitializeLatency = false;
	float HUDLatency;

	float HighPingRunningTime = 0.0f;
	UPROPERTY(EditDefaultsOnly, Category = Ping)
	float HighPingDuration = 5.0f;
	UPROPERTY(EditDefaultsOnly, Category = Ping)
	float CheckPingFrequency = 10.0f;
	UPROPERTY(EditDefaultsOnly, Category = Ping)
	float HighPingThreshold = 50.0f;
	float PingAnimationRunningTime = 0.0f;

	bool bInitializeTeamScore = false;
	int HUDRedScore;
	int HUDBlueScore;

#pragma endregion Internal HUD Setup

#pragma region Latency Ping
	UFUNCTION(Server, Reliable)
	void ServerReportPingStatus(bool bHighPing);
#pragma endregion Latency Ping

};
