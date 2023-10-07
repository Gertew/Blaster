// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionsMenu.generated.h"

class UButton;
class APlayerController;
class UMultiplayerSessionsSubsystem;

/**
 * 
 */
UCLASS()
class BLASTER_API UOptionsMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void MenuSetup();
	void MenuTeardown();

protected:
	virtual bool Initialize() override;
	
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);

	UFUNCTION()
	void OnPlayerLeftGame();

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MainMenuButton;

	UFUNCTION()
	void MainMenuButtonClicked();

	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY()
	TObjectPtr<UMultiplayerSessionsSubsystem> MultiplayerSessionsSubsystem;

public:

};
