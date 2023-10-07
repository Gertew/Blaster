// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h"

void UCharacterOverlay::SetHealthPercent(float Percent)
{
	if (HealthBar && HealthText)
	{
		HealthBar->SetPercent(Percent);
		FText TextPercent = FText::FromString(FString::Printf(TEXT("%d%%"), (uint32)(Percent * 100)));
		HealthText->SetText(TextPercent);
	}
}

void UCharacterOverlay::SetShieldPercent(float Percent)
{
	if (ShieldBar && ShieldText)
	{
		ShieldBar->SetPercent(Percent);
		FText TextPercent = FText::FromString(FString::Printf(TEXT("%d%%"), (uint32)(Percent * 100)));
		ShieldText->SetText(TextPercent);
	}
}

void UCharacterOverlay::SetScoreAmount(float Score)
{
	if (ScoreAmount)
	{
		FText TextAmount = FText::FromString(FString::Printf(TEXT("%d"), FMath::FloorToInt(Score)));
		ScoreAmount->SetText(TextAmount);
	}
}

void UCharacterOverlay::SetRedTeamScore(int32 Score)
{
	if (RedTeamScore)
	{
		FText TextAmount = FText::FromString(FString::Printf(TEXT("%d"), Score));
		RedTeamScore->SetText(TextAmount);
	}
}

void UCharacterOverlay::SetBlueTeamScore(int32 Score)
{
	if (BlueTeamScore)
	{
		FText TextAmount = FText::FromString(FString::Printf(TEXT("%d"), Score));
		BlueTeamScore->SetText(TextAmount);
	}
}

void UCharacterOverlay::HideTeamScores()
{
	if (RedTeamBox && BlueTeamBox && RedTeamScore && BlueTeamScore)
	{
		RedTeamScore->SetText(FText::GetEmpty());
		RedTeamBox->SetVisibility(ESlateVisibility::Collapsed);
		BlueTeamScore->SetText(FText::GetEmpty());
		BlueTeamBox->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UCharacterOverlay::SetDefeatsAmount(int32 Defeats)
{
	if (DefeatsAmount)
	{
		FText TextDefeats = FText::FromString(FString::Printf(TEXT("%d"), Defeats));
		DefeatsAmount->SetText(TextDefeats);
	}
}

void UCharacterOverlay::SetWeaponAmmoAmount(int32 Ammo)
{
	if (WeaponAmmoAmount)
	{
		FText TextWeaponAmmo = FText::FromString(FString::Printf(TEXT("%d"), Ammo));
		WeaponAmmoAmount->SetText(TextWeaponAmmo);
	}
}

void UCharacterOverlay::SetCarriedAmmoAmount(int32 Ammo)
{
	if (CarriedAmmoAmount)
	{
		FText TextCarriedAmmo = FText::FromString(FString::Printf(TEXT("%d"), Ammo));
		CarriedAmmoAmount->SetText(TextCarriedAmmo);
	}
}

void UCharacterOverlay::UpdateElimPlayer(const FString& Player)
{
	if (ElimText && ElimPlayer)
	{
		FText TextElimPlayer = FText::FromString(Player);
		ElimPlayer->SetText(TextElimPlayer);
		ElimPlayer->SetVisibility(ESlateVisibility::Visible);
		ElimText->SetVisibility(ESlateVisibility::Visible);
	}
}

void UCharacterOverlay::HideElimPlayer()
{
	if (ElimText && ElimPlayer)
	{
		ElimPlayer->SetText(FText::GetEmpty());
		ElimPlayer->SetVisibility(ESlateVisibility::Collapsed);
		ElimText->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UCharacterOverlay::SetWeaponIcon(UTexture2D* Icon)
{
	if (WeaponIcon)
	{
		FLinearColor Color = WeaponIcon->GetColorAndOpacity();
		if (Icon)
		{
			Color.A = 1.0f;
			WeaponIcon->SetBrushFromTexture(Icon);
		}
		else
		{
			Color.A = 0.0f;
		}
		WeaponIcon->SetColorAndOpacity(Color);
	}
}

void UCharacterOverlay::SetMatchCountdown(float CountdownTime)
{
	if (MatchCountdownText)
	{
		if (CountdownTime < 0.0f)
		{
			MatchCountdownText->SetText(FText());
			return;
		}

		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.0f);
		int32 Seconds = CountdownTime - Minutes * 60;
		FText TextCountdown = FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds));
		MatchCountdownText->SetText(TextCountdown);
	}
}

void UCharacterOverlay::SetLatency(float Latency)
{
	if (LatencyText)
	{
		FText TextLatency = FText::FromString(FString::Printf(TEXT("%d ms"), FMath::FloorToInt(Latency)));
		LatencyText->SetText(TextLatency);
	}
}

void UCharacterOverlay::SetGrenadesAmount(int32 Grenades)
{
	if (GrenadesAmount)
	{
		FText TextGrenades = FText::FromString(FString::Printf(TEXT("%d"), Grenades));
		GrenadesAmount->SetText(TextGrenades);
	}
}

void UCharacterOverlay::PlayHighPingAnimation()
{
	if (WifiImage && HighPingAnimation)
	{
		WifiImage->SetOpacity(1.0f);
		PlayAnimation(HighPingAnimation, 0.0f, 5);
	}
}

void UCharacterOverlay::StopHighPingAnimation()
{
	if (WifiImage && HighPingAnimation)
	{
		WifiImage->SetOpacity(0.0f);
		if (IsAnimationPlaying(HighPingAnimation))
		{
			StopAnimation(HighPingAnimation);
		}
	}
}
