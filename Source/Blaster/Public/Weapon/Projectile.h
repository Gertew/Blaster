// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

#pragma region Forward Declarations
class UBoxComponent;
class UProjectileMovementComponent;
class UParticleSystem;
class UParticleSystemComponent;
class USoundCue;
class UNiagaraSystem;
class UNiagaraComponent;
#pragma endregion Forward Declarations

UCLASS()
class BLASTER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

#pragma region Server Side Rewind
	bool bUseServerSideRewind = false;
	FVector_NetQuantize TraceStart;
	FVector_NetQuantize100 InitialVelocity;
	UPROPERTY(EditDefaultsOnly)
	float InitialSpeed = 30000.0f;
#pragma endregion Server Side Rewind

#pragma region Damage
	UPROPERTY(EditDefaultsOnly)
	float Damage = 1.0f;
	UPROPERTY(EditDefaultsOnly)
	float HeadshotDamageMultiplier = 1.0f;
#pragma endregion Damage

protected:
	virtual void BeginPlay() override;

#pragma region Components
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> CollisionBox;

	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
#pragma endregion Components

#pragma region Projectile Hit
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnHit(bool bHitPlayer);
#pragma endregion Projectile Hit

#pragma region Effects
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UParticleSystem> Tracer;
	UPROPERTY()
	TObjectPtr<UParticleSystemComponent> TracerComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UParticleSystem> ImpactParticles;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundCue> ImpactSound;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UParticleSystem> PlayerImpactParticles;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundCue> PlayerImpactSound;

	virtual void SpawnImpactEffects(bool bHitPlayer);
#pragma endregion Effects

#pragma region Effects
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> TrailSystem;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> TrailSystemComponent;

	void SpawnTrailSystem();
#pragma endregion Effects

#pragma region Explosion
	UPROPERTY(EditDefaultsOnly)
	float DestroyTime = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> ProjectileMesh;

	UPROPERTY(EditDefaultsOnly)
	float InnerDamageRadius = 200.0f;
	UPROPERTY(EditDefaultsOnly)
	float OuterDamageRadius = 500.0f;
	UPROPERTY(EditDefaultsOnly)
	float LaunchMagnitude = 500.0f;
	UPROPERTY(EditDefaultsOnly)
	float ZLaunchMultiplier = 2.0f;

	void ExplodeDamage(AActor* OtherActor);
#pragma endregion Explosion

public:	
#pragma region Getters/Setters
	FORCEINLINE float GetHeadshotDamage() { return Damage * HeadshotDamageMultiplier; }
#pragma endregion Getters/Setters

};
