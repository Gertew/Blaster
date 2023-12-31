// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/OverheadWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	ENetRole RemoteRole = InPawn->GetRemoteRole();
	FString Role;
	switch (RemoteRole)
	{
	case ENetRole::ROLE_Authority:
		Role = FString("Authority");
		break;
	case ENetRole::ROLE_AutonomousProxy:
		Role = FString("Autonomous Proxy");
		break;
	case ENetRole::ROLE_SimulatedProxy:
		Role = FString("Simulated Proxy");
		break;
	case ENetRole::ROLE_None:
		Role = FString("None");
		break;
	}

	APlayerState* PlayerState = InPawn->GetPlayerState();
	FString PlayerName = "Unknown";
	if (PlayerState)
	{
		PlayerName = PlayerState->GetPlayerName();
	}

	FString RemoteRoleString = FString::Printf(TEXT("PlayerName: %s - Remote Role: %s"), *PlayerName, *Role);
	SetDisplayText(PlayerName);
}

void UOverheadWidget::NativeDestruct()
{
	Super::NativeDestruct();

	RemoveFromParent();
}
