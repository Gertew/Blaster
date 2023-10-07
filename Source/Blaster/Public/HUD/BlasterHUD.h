// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"

class UTexture2D;
class UCharacterOverlay;
class UUserWidget;
class UAnnouncement;
class USettings;
class UEliminatedAnnouncement;

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()

public:
	TObjectPtr<UTexture2D> CrosshairCenter;
	TObjectPtr<UTexture2D> CrosshairLeft;
	TObjectPtr<UTexture2D> CrosshairRight;
	TObjectPtr<UTexture2D> CrosshairTop;
	TObjectPtr<UTexture2D> CrosshairBottom;
	float CrosshairSpread;
	FLinearColor CrosshairColor;
};


/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void AddCharacterOverlay();
	void AddAnnouncement();
	void AddEliminatedAnnouncement(FString Attacker, FString Victim);
	virtual void DrawHUD() override;

	UPROPERTY(EditDefaultsOnly, Category = "Player Stats")
	TSubclassOf<UUserWidget> CharacterOverlayClass;
	UPROPERTY()
	TObjectPtr<UCharacterOverlay> CharacterOverlay;

	UPROPERTY(EditDefaultsOnly, Category = "Announcements")
	TSubclassOf<UUserWidget> AnnouncementClass;
	UPROPERTY()
	TObjectPtr<UAnnouncement> Announcement;
protected:
	virtual void BeginPlay() override;

private:
	FHUDPackage HUDPackage;
	UPROPERTY(EditDefaultsOnly)
	float CrosshairSpreadMax = 25.0f;

	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UEliminatedAnnouncement> EliminatedAnnouncementClass;

	UPROPERTY()
	TObjectPtr<APlayerController> OwningPlayer;

	UPROPERTY(EditDefaultsOnly)
	float EliminatedAnnouncementTime = 2.5f;

	UPROPERTY()
	TArray<UEliminatedAnnouncement*> EliminatedMessages;

	UFUNCTION()
	void EliminatedAnnouncementTimerFinished(UEliminatedAnnouncement* MessageToRemove);

public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; }

};
