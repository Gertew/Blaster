// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "HitScanWeapon.generated.h"

class UParticleSystem;

/**
 * 
 */
UCLASS()
class BLASTER_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:
	virtual void Fire(const FVector& HitTarget) override;
protected:
	void WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit);

	virtual void SpawnImpactEffects(const FHitResult& HitResult);
	virtual void SpawnFireEffects(const FHitResult& HitResult);


private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UParticleSystem> ImpactParticles;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundCue> ImpactSound;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UParticleSystem> BeamParticles;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UParticleSystem> MuzzleFlash;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundCue> FireSound;


public:

};
