// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ProjectileGrenade.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

AProjectileGrenade::AProjectileGrenade()
{
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrenadeMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->SetIsReplicated(true);
	ProjectileMovement->bShouldBounce = true;

	ProjectileMovement->InitialSpeed = InitialSpeed;
	ProjectileMovement->MaxSpeed = InitialSpeed;
}

void AProjectileGrenade::BeginPlay()
{
	AActor::BeginPlay();

	Damage = GrenadeDamage;
	ProjectileMovement->OnProjectileBounce.AddDynamic(this, &AProjectileGrenade::OnBounce);

	SpawnTrailSystem();
	SetLifeSpan(DestroyTime);
}

#if WITH_EDITOR
void AProjectileGrenade::PostEditChangeProperty(FPropertyChangedEvent& Event)
{
	Super::PostEditChangeProperty(Event);

	FName PropertyName = Event.Property != nullptr ? Event.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(AProjectileGrenade, InitialSpeed))
	{
		if (ProjectileMovement)
		{
			ProjectileMovement->InitialSpeed = InitialSpeed;
			ProjectileMovement->MaxSpeed = InitialSpeed;
		}
	}
}
#endif

void AProjectileGrenade::Destroyed()
{
	SpawnImpactEffects(false);
	ExplodeDamage(nullptr);

	Super::Destroyed();
}

void AProjectileGrenade::OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	if (BounceSound && ImpactVelocity.Length() > 100.0f)
	{
		UGameplayStatics::PlaySoundAtLocation(this, BounceSound, ImpactResult.ImpactPoint); 
	}

}