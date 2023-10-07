// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

#pragma region Forward Declarations
class UProgressBar;
class UTextBlock;
class UImage;
class UHorizontalBox;
#pragma endregion Forward Declarations

/**
 * 
 */
UCLASS()
class BLASTER_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()
	

public:
#pragma region Player Attributes
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HealthText;

	void SetHealthPercent(float Percent);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ShieldBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ShieldText;

	void SetShieldPercent(float Percent);
#pragma endregion Player Attributes

#pragma region Scores
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ScoreAmount;

	void SetScoreAmount(float Score);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> RedTeamBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> RedTeamScore;

	void SetRedTeamScore(int32 Score);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> BlueTeamBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> BlueTeamScore;

	void SetBlueTeamScore(int32 Score);

	void HideTeamScores();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DefeatsAmount;

	void SetDefeatsAmount(int32 Defeats);

#pragma endregion Scores

#pragma region Weapon/Ammo
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> WeaponAmmoAmount;

	void SetWeaponAmmoAmount(int32 Ammo);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CarriedAmmoAmount;

	void SetCarriedAmmoAmount(int32 Ammo);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> WeaponIcon;
	void SetWeaponIcon(UTexture2D* Icon);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> GrenadesAmount;
	void SetGrenadesAmount(int32 Grenades);
#pragma endregion Weapon/Ammo

#pragma region Elimination
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ElimText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ElimPlayer;

	void UpdateElimPlayer(const FString& Player);
	void HideElimPlayer();
#pragma endregion Elimination

#pragma region Match Info
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MatchCountdownText;
	void SetMatchCountdown(float CountdownTime);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LatencyText;
	void SetLatency(float Latency);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> WifiImage;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> HighPingAnimation;
	void PlayHighPingAnimation();
	void StopHighPingAnimation();
#pragma endregion Match Info

};
