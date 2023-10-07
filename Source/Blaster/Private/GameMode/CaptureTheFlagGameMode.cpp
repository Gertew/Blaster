// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/CaptureTheFlagGameMode.h"
#include "CaptureTheFlag/FlagZone.h"
#include "Weapon/Flag.h"
#include "GameState/BlasterGameState.h"

void ACaptureTheFlagGameMode::PlayerEliminated(ABlasterCharacter* VictimCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController)
{
	ABlasterGameMode::PlayerEliminated(VictimCharacter, VictimController, AttackerController);
}

void ACaptureTheFlagGameMode::FlagCaptured(AFlag* Flag, AFlagZone* Zone)
{
	if (Flag && Zone)
	{
		bool bValidCapture = Flag->GetTeam() != Zone->Team;
		if (bValidCapture)
		{
			ABlasterGameState* BlasterGameState = Cast<ABlasterGameState>(GameState);
			if (BlasterGameState)
			{
				if (Zone->Team == ETeam::ET_BlueTeam)
				{
					BlasterGameState->BlueTeamAddScore();
				}
				if (Zone->Team == ETeam::ET_RedTeam)
				{
					BlasterGameState->RedTeamAddScore();
				}
			}
		}
	}
}
