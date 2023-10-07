// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/Pickup.h"
#include "SpeedPickup.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ASpeedPickup : public APickup
{
	GENERATED_BODY()
	
public:

protected:
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;


private:
	UPROPERTY(EditDefaultsOnly)
	float BaseSpeedBuff = 1600.0f;
	UPROPERTY(EditDefaultsOnly)
	float CrouchSpeedBuff = 850.0f;
	UPROPERTY(EditDefaultsOnly)
	float SpeedBuffTime = 30.0f;

public:

};
