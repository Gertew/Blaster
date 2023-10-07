// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/BlasterPlayerController.h"
#include "HUD/BlasterHUD.h"
#include "HUD/CharacterOverlay.h"
#include "HUD/Announcement.h"
#include "Character/BlasterCharacter.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "GameMode/BlasterGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "BlasterComponents/CombatComponent.h"
#include "GameState/BlasterGameState.h"
#include "PlayerState/BlasterPlayerState.h"
#include "InputModifiers.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "HUD/OptionsMenu.h"
#include "BlasterTypes/Announcement.h"


void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	BlasterHUD = Cast<ABlasterHUD>(GetHUD());
	ServerCheckMatchState();
}

void ABlasterPlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (BlasterHUD && BlasterHUD->CharacterOverlay)
		{
			CharacterOverlay = BlasterHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				if (bInitializeHealth) SetHUDHealth(HUDHealth, HUDMaxHealth);
				if (bInitializeShield) SetHUDShield(HUDShield, HUDMaxShield);
				if (bInitializeScore) SetHUDScore(HUDScore);
				if (bInitializeDefeats) SetHUDDefeats(HUDDefeats);
				if (bInitializeCarriedAmmo) SetHUDCarriedAmmo(HUDCarriedAmmo);
				if (bInitializeWeaponAmmo) SetHUDWeaponAmmo(HUDWeaponAmmo);
				if (bInitializeWeaponIcon) SetHUDWeaponIcon(HUDWeaponIcon);
				if (bInitializeLatency) SetHUDLatency(HUDLatency);
				if (bInitializeTeamScore) InitHUDTeamScores();

				if (bShowTeamScores)
				{
					InitHUDTeamScores();
				}
				else
				{
					HideHUDTeamScores();
				}

				ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());
				if (BlasterCharacter && BlasterCharacter->GetCombatComponent())
				{
					if (bInitializeGrenades) SetHUDGrenades(BlasterCharacter->GetCombatComponent()->GetGrenades());
				}
			}
		}
	}
}

void ABlasterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (InputComponent == nullptr)
	{
		return;
	}

	if (TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(OptionsMenuAction, ETriggerEvent::Triggered, this, &ABlasterPlayerController::ShowOptionsMenu);
	}
}

void ABlasterPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerController, MatchState);
	DOREPLIFETIME(ABlasterPlayerController, bShowTeamScores);
}

void ABlasterPlayerController::SetHUDTime()
{
	float TimeLeft = 0.0f;
	if (MatchState == MatchState::WaitingToStart)
	{
		TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	}
	else if (MatchState == MatchState::InProgress)
	{
		TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	}
	else if (MatchState == MatchState::Cooldown)
	{
		TimeLeft = CooldownTime + WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	}

	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);

	if (HasAuthority())
	{
		BlasterGameMode = BlasterGameMode == nullptr ? Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this)) : BlasterGameMode;
		if (BlasterGameMode)
		{
			LevelStartingTime = BlasterGameMode->LevelStartingTime;
			SecondsLeft = FMath::CeilToInt(BlasterGameMode->GetCountdownTime() + LevelStartingTime);
		}
	}

	if (CountdownInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
		{
			SetHUDAnnouncementCountdown(TimeLeft);
		}
		else if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(TimeLeft);
		}
	}
	CountdownInt = SecondsLeft;
}

void ABlasterPlayerController::ServerRequestServerTime_Implementation(float ClientRequestTime)
{
	float CurrentServerTime = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(ClientRequestTime, CurrentServerTime);
}

void ABlasterPlayerController::ClientReportServerTime_Implementation(float ClientRequestTime, float ServerReceievedClientRequestTime)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - ClientRequestTime;
	SingleTripTime = 0.5f * RoundTripTime;
	float CurrentServerTime = ServerReceievedClientRequestTime + (0.5f * RoundTripTime);
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

void ABlasterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(InPawn);
	if (BlasterCharacter)
	{
		SetHUDHealth(BlasterCharacter->GetHealth(), BlasterCharacter->GetMaxHealth());
		SetHUDShield(BlasterCharacter->GetShield(), BlasterCharacter->GetMaxShield());
		if (BlasterCharacter->GetCombatComponent())
		{
			SetHUDGrenades(BlasterCharacter->GetCombatComponent()->GetGrenades());
			SetHUDWeaponAmmo(BlasterCharacter->GetCombatComponent()->GetEquippedWeaponAmmo());
			SetHUDCarriedAmmo(BlasterCharacter->GetCombatComponent()->GetCarriedAmmo());
			SetHUDWeaponIcon(BlasterCharacter->GetCombatComponent()->GetEquippedWeaponIcon());
		}
	}
	HideHUDElim();
	if (PlayerState)
	{
		SetHUDLatency(PlayerState->GetPingInMilliseconds());
	}
}

void ABlasterPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetHUDTime();

	CheckTimeSync(DeltaTime);
	PollInit();

	CheckPing(DeltaTime);
}

void ABlasterPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.0f;
	}
}

void ABlasterPlayerController::HighPingWarning()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD)
	{
		if (BlasterHUD->CharacterOverlay)
		{
			BlasterHUD->CharacterOverlay->PlayHighPingAnimation();
		}
	}
}

void ABlasterPlayerController::StopHighPingWarning()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD)
	{
		if (BlasterHUD->CharacterOverlay)
		{
			BlasterHUD->CharacterOverlay->StopHighPingAnimation();
		}
	}
}

void ABlasterPlayerController::CheckPing(float DeltaTime)
{
	HighPingRunningTime += DeltaTime;
	if (HighPingRunningTime > CheckPingFrequency)
	{
		PlayerState = PlayerState == nullptr ? GetPlayerState<APlayerState>() : PlayerState;
		if (PlayerState)
		{
			float Ping = PlayerState->ExactPing;
			if (Ping > HighPingThreshold)
			{
				HighPingWarning();
				PingAnimationRunningTime = 0.0f;
				ServerReportPingStatus(true);
			}
			else
			{
				ServerReportPingStatus(false);
			}
		}
		HighPingRunningTime = 0.0f;

		bool bHighPingAnimationPlaying =
			BlasterHUD && BlasterHUD->CharacterOverlay &&
			BlasterHUD->CharacterOverlay->HighPingAnimation &&
			BlasterHUD->CharacterOverlay->IsAnimationPlaying(BlasterHUD->CharacterOverlay->HighPingAnimation);
		if (bHighPingAnimationPlaying)
		{
			PingAnimationRunningTime += DeltaTime;
			if (PingAnimationRunningTime > HighPingDuration)
			{
				StopHighPingWarning();
			}
		}
	}
}

void ABlasterPlayerController::ShowOptionsMenu()
{
	if (OptionsMenuClass == nullptr)
	{
		return;
	}

	if (OptionsMenu == nullptr)
	{
		OptionsMenu = CreateWidget<UOptionsMenu>(this, OptionsMenuClass);
	}

	if (OptionsMenu)
	{
		bOptionsMenuOpen = !bOptionsMenuOpen;
		if (bOptionsMenuOpen)
		{
			OptionsMenu->MenuSetup();
		}
		else
		{
			OptionsMenu->MenuTeardown();
		}
	}
}

void ABlasterPlayerController::OnRep_ShowTeamScores()
{
	if (bShowTeamScores)
	{
		InitHUDTeamScores();
	}
}

void ABlasterPlayerController::ServerReportPingStatus_Implementation(bool bHighPing)
{
	HighPingDelegate.Broadcast(bHighPing);
}


void ABlasterPlayerController::ServerCheckMatchState_Implementation()
{
	ABlasterGameMode* GameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		CooldownTime = GameMode->CooldownTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		MatchState = GameMode->GetMatchState();
		bShowTeamScores = GameMode->IsTeamsMatch();
		ClientJoinMidgame(MatchState, WarmupTime, MatchTime, CooldownTime, LevelStartingTime, bShowTeamScores);

		if (BlasterHUD && MatchState == MatchState::WaitingToStart)
		{
			BlasterHUD->AddAnnouncement();
		}
	}
}
void ABlasterPlayerController::ClientJoinMidgame_Implementation(FName StateOfMatch, float TimeOfWarmup, float TimeOfMatch, float TimeOfCooldown, float TimeOfStart, bool bIsTeamsMatch)
{
	WarmupTime = TimeOfWarmup;
	MatchTime = TimeOfMatch;
	CooldownTime = TimeOfCooldown;
	LevelStartingTime = TimeOfStart;
	MatchState = StateOfMatch;
	bShowTeamScores = bIsTeamsMatch;
	OnMatchStateSet(MatchState);

	if (BlasterHUD && MatchState == MatchState::WaitingToStart)
	{
		BlasterHUD->AddAnnouncement();
	}
}

float ABlasterPlayerController::GetServerTime()
{
	if (HasAuthority())
	{
		return GetWorld()->GetTimeSeconds();
	}
	else
	{
		return GetWorld()->GetTimeSeconds() + ClientServerDelta;
	}
}

void ABlasterPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void ABlasterPlayerController::OnMatchStateSet(FName State, bool bTeamsMatch)
{
	MatchState = State;

	if (MatchState == MatchState::WaitingToStart)
	{

	}
	else if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted(bTeamsMatch);
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void ABlasterPlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::WaitingToStart)
	{

	}
	else if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void ABlasterPlayerController::HandleMatchHasStarted(bool bTeamsMatch)
{
	if (HasAuthority())
	{
		bShowTeamScores = bTeamsMatch;
	}

	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD)
	{
		if (BlasterHUD->CharacterOverlay == nullptr)
		{
			BlasterHUD->AddCharacterOverlay();
		}
		if (BlasterHUD->Announcement)
		{
			BlasterHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (!HasAuthority())
	{
		return;
	}

	if (bTeamsMatch)
	{
		InitHUDTeamScores();
	}
}

FText ABlasterPlayerController::GetInfoText(const TArray<ABlasterPlayerState*>& Players)
{
	ABlasterPlayerState* BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
	if (BlasterPlayerState == nullptr) {
		return FText();
	}

	FString InfoString;
	if (Players.IsEmpty())
	{
		InfoString = Announcement::ThereIsNoWinner;
	}
	else if (Players.Num() == 1 && Players[0] == BlasterPlayerState)
	{
		InfoString = Announcement::YouAreTheWinner;

	}
	else if (Players.Num() == 1)
	{
		InfoString = FString::Printf(TEXT("Winner \n%s"), *Players[0]->GetPlayerName());
	}
	else if (Players.Num() > 1)
	{
		FString TempString = Announcement::PlayersTiedForTheWin;
		TempString.Append(FString("\n"));
		for (ABlasterPlayerState* TiedPlayer : Players)
		{
			TempString.Append(FString::Printf(TEXT("%s\n"), *TiedPlayer->GetPlayerName()));
		}
		InfoString = TempString;
	}

	return FText::FromString(InfoString);
}

FText ABlasterPlayerController::GetTeamsInfoText(ABlasterGameState* BlasterGameState)
{
	if (BlasterGameState == nullptr)
	{
		return FText();
	}
	FString InfoString;

	const int32 RedTeamScore = BlasterGameState->RedTeamScore;
	const int32 BlueTeamScore = BlasterGameState->BlueTeamScore;

	if (RedTeamScore == 0 && BlueTeamScore == 0)
	{
		InfoString = Announcement::ThereIsNoWinner;
	}
	else if (RedTeamScore == BlueTeamScore)
	{
		InfoString = FString::Printf(TEXT("%s\n"), *Announcement::TeamsTiedForTheWin);
		InfoString.Append(Announcement::RedTeam);
		InfoString.Append(FString("\n"));
		InfoString.Append(Announcement::BlueTeam);
		InfoString.Append(FString("\n"));
	}
	else if (RedTeamScore > BlueTeamScore)
	{
		InfoString = Announcement::RedTeamWins;
		InfoString.Append(FString("\n"));
		InfoString.Append(FString::Printf(TEXT("%s: %d\n"), *Announcement::RedTeam, RedTeamScore));
		InfoString.Append(FString::Printf(TEXT("%s: %d\n"), *Announcement::BlueTeam, BlueTeamScore));
	}
	else if (BlueTeamScore > RedTeamScore)
	{
		InfoString = Announcement::BlueTeamWins;
		InfoString.Append(FString("\n"));
		InfoString.Append(FString::Printf(TEXT("%s: %d\n"), *Announcement::BlueTeam, BlueTeamScore));
		InfoString.Append(FString::Printf(TEXT("%s: %d\n"), *Announcement::RedTeam, RedTeamScore));
	}

	return FText::FromString(InfoString);
}

void ABlasterPlayerController::HandleCooldown()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD)
	{
		BlasterHUD->CharacterOverlay->RemoveFromParent();
		if (BlasterHUD->Announcement)
		{
			BlasterHUD->Announcement->SetVisibility(ESlateVisibility::Visible);
			FString AnnouncementText = Announcement::NewMatchStartsIn;
			BlasterHUD->Announcement->SetAnnouncement(FText::FromString(AnnouncementText));

			ABlasterGameState* BlasterGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
			if (BlasterGameState)
			{
				TArray<ABlasterPlayerState*> TopPlayers = BlasterGameState->TopScoringPlayers;
				ABlasterPlayerState* BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
				FText InfoText = bShowTeamScores ? GetTeamsInfoText(BlasterGameState) : GetInfoText(TopPlayers);

				BlasterHUD->Announcement->SetInfo(InfoText);
			}
		}
	}
	
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());
	if (BlasterCharacter && BlasterCharacter->GetCombatComponent())
	{
		BlasterCharacter->bDisableGameplay = true;
		BlasterCharacter->GetCombatComponent()->Fire(false);
		BlasterCharacter->GetCombatComponent()->SetAiming(false);
	}
}

void ABlasterPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD)
	{
		if (BlasterHUD->CharacterOverlay)
		{
			const float HealthPercent = Health / MaxHealth;
			BlasterHUD->CharacterOverlay->SetHealthPercent(HealthPercent);
		}
		else
		{
			bInitializeHealth = true;
			HUDHealth = Health;
			HUDMaxHealth = MaxHealth;
		}
	}
}

void ABlasterPlayerController::SetHUDShield(float Shield, float MaxShield)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD)
	{
		if (BlasterHUD->CharacterOverlay)
		{
			const float HealthPercent = Shield / MaxShield;
			BlasterHUD->CharacterOverlay->SetShieldPercent(HealthPercent);
		}
		else
		{
			bInitializeShield = true;
			HUDShield = Shield;
			HUDMaxShield = MaxShield;
		}
	}
}

void ABlasterPlayerController::SetHUDScore(float Score)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD)
	{
		if (BlasterHUD->CharacterOverlay)
		{
			BlasterHUD->CharacterOverlay->SetScoreAmount(Score);
		}
		else
		{
			bInitializeScore = true;
			HUDScore = Score;
		}
	}
}

void ABlasterPlayerController::SetHUDDefeats(int32 Defeats)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD)
	{
		if (BlasterHUD->CharacterOverlay)
		{
			BlasterHUD->CharacterOverlay->SetDefeatsAmount(Defeats);
		}
		else
		{
			bInitializeDefeats = true;
			HUDDefeats = Defeats;
		}
	}
}


void ABlasterPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD)
	{
		if (BlasterHUD->CharacterOverlay)
		{
			BlasterHUD->CharacterOverlay->SetWeaponAmmoAmount(Ammo);
		}
		else
		{
			bInitializeWeaponAmmo = true;
			HUDWeaponAmmo = Ammo;
		}
	}
}

void ABlasterPlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD)
	{
		if (BlasterHUD->CharacterOverlay)
		{
			BlasterHUD->CharacterOverlay->SetCarriedAmmoAmount(Ammo);
		}
		else
		{
			bInitializeCarriedAmmo = true;
			HUDCarriedAmmo = Ammo;
		}
	}
}

void ABlasterPlayerController::SetHUDGrenades(int32 Grenades)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD)
	{
		if (BlasterHUD->CharacterOverlay)
		{
			BlasterHUD->CharacterOverlay->SetGrenadesAmount(Grenades);
		}
		else
		{
			bInitializeGrenades = true;
			HUDGrenades = Grenades;
		}
	}
}

void ABlasterPlayerController::SetHUDWeaponIcon(UTexture2D* Icon)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD)
	{
		if (BlasterHUD->CharacterOverlay)
		{
			BlasterHUD->CharacterOverlay->SetWeaponIcon(Icon);
		}
		else
		{
			bInitializeWeaponIcon = true;
			HUDWeaponIcon = Icon;
		}
	}
}

void ABlasterPlayerController::SetHUDMatchCountdown(float CountdownTime)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD)
	{
		if (BlasterHUD->CharacterOverlay)
		{
			BlasterHUD->CharacterOverlay->SetMatchCountdown(CountdownTime);
		}
	}
}

void ABlasterPlayerController::SetHUDAnnouncementCountdown(float CountdownTime)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD)
	{
		if (BlasterHUD->Announcement)
		{
			BlasterHUD->Announcement->SetAnnouncementCountdown(CountdownTime);
		}
	}
}

void ABlasterPlayerController::SetHUDLatency(float Latency)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD)
	{
		if (BlasterHUD->CharacterOverlay)
		{
			BlasterHUD->CharacterOverlay->SetLatency(Latency);
		}
		else
		{
			bInitializeLatency = true;
			HUDLatency = Latency;
		}
	}
}

void ABlasterPlayerController::UpdateHUDElim(const FString& AttackingPlayer)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD)
	{
		if (BlasterHUD->CharacterOverlay)
		{
			BlasterHUD->CharacterOverlay->UpdateElimPlayer(AttackingPlayer);
		}
	}
}

void ABlasterPlayerController::HideHUDElim()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD)
	{
		if (BlasterHUD->CharacterOverlay)
		{
			BlasterHUD->CharacterOverlay->HideElimPlayer();
		}
	}
}

void ABlasterPlayerController::HideHUDTeamScores()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD)
	{
		if (BlasterHUD->CharacterOverlay)
		{
			BlasterHUD->CharacterOverlay->HideTeamScores();
		}
	}
}

void ABlasterPlayerController::InitHUDTeamScores()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD)
	{
		if (BlasterHUD->CharacterOverlay)
		{
			BlasterHUD->CharacterOverlay->SetRedTeamScore(0);
			BlasterHUD->CharacterOverlay->SetBlueTeamScore(0);
		}
		else
		{
			bInitializeTeamScore = true;
		}
	}
}

void ABlasterPlayerController::SetHUDRedTeamScore(int32 Score)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD)
	{
		if (BlasterHUD->CharacterOverlay)
		{
			BlasterHUD->CharacterOverlay->SetRedTeamScore(Score);
		}
	}
}

void ABlasterPlayerController::SetHUDBlueTeamScore(int32 Score)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	if (BlasterHUD)
	{
		if (BlasterHUD->CharacterOverlay)
		{
			BlasterHUD->CharacterOverlay->SetBlueTeamScore(Score);
		}
	}
}

void ABlasterPlayerController::BroadcastElimination(APlayerState* Attacker, APlayerState* Victim)
{
	ClientElimination(Attacker, Victim);
}


void ABlasterPlayerController::ClientElimination_Implementation(APlayerState* Attacker, APlayerState* Victim)
{
	APlayerState* CurrentPlayer = GetPlayerState<APlayerState>();
	if (Attacker && Victim && CurrentPlayer)
	{
		BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
		if (BlasterHUD)
		{
			if (Attacker == CurrentPlayer && Victim != CurrentPlayer)
			{
				BlasterHUD->AddEliminatedAnnouncement("You", Victim->GetPlayerName());
				return;
			}
			if (Attacker != CurrentPlayer && Victim == CurrentPlayer)
			{
				BlasterHUD->AddEliminatedAnnouncement(Attacker->GetPlayerName(), "You");
				return;
			}
			if (Attacker == CurrentPlayer && Victim == CurrentPlayer)
			{
				BlasterHUD->AddEliminatedAnnouncement("You", "yourself");
				return;
			}
			if (Attacker == Victim && Attacker != CurrentPlayer)
			{
				BlasterHUD->AddEliminatedAnnouncement(Attacker->GetPlayerName(), "themselves");
				return;
			}

			BlasterHUD->AddEliminatedAnnouncement(Attacker->GetPlayerName(), Victim->GetPlayerName());
		}
	}
}
