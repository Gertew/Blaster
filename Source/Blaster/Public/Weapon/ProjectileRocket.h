// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Projectile.h"
#include "ProjectileRocket.generated.h"

class UAudioComponent;
class USoundAttenuation;
class URocketMovementComponent;

/**
 * 
 */
UCLASS()
class BLASTER_API AProjectileRocket : public AProjectile
{
	GENERATED_BODY()
	
public:
	AProjectileRocket();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& Event) override;
#endif

protected:
	virtual void BeginPlay() override;
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void SpawnImpactEffects(bool bHitPlayer) override;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundCue> LoopSound;

	UPROPERTY()
	TObjectPtr<UAudioComponent> SoundLoopComponent;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundAttenuation> LoopSoundAttenuation;

	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<URocketMovementComponent> RocketMovement;
private:

};
