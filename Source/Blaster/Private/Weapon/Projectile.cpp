// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Projectile.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "Character/BlasterCharacter.h"
#include "Blaster/Blaster.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECollisionResponse::ECR_Block);

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	CollisionBox->IgnoreActorWhenMoving(GetOwner(), true);
	
	if (Tracer)
	{
		TracerComponent = UGameplayStatics::SpawnEmitterAttached(
			Tracer,
			CollisionBox,
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition
		);
	}

	if (HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	}
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (HasAuthority())
	{
		ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
		bool bHitPlayer = BlasterCharacter ? true : false;
		Multicast_OnHit(bHitPlayer);
	}

	Destroy();
}

void AProjectile::Multicast_OnHit_Implementation(bool bHitPlayer)
{
	SpawnImpactEffects(bHitPlayer);
}

void AProjectile::SpawnImpactEffects(bool bHitPlayer)
{
	UParticleSystem* Particles = bHitPlayer ? PlayerImpactParticles : ImpactParticles;
	USoundCue* Sound = bHitPlayer ? PlayerImpactSound : ImpactSound;

	if (Particles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Particles, GetActorTransform());
	}

	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, GetActorLocation());
	}
}

void AProjectile::SpawnTrailSystem()
{
	if (TrailSystem)
	{
		TrailSystemComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			TrailSystem,
			GetRootComponent(),
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition,
			false
		);
	}
}

void AProjectile::ExplodeDamage(AActor* OtherActor)
{
	APawn* FiringPawn = GetInstigator();
	if (FiringPawn && HasAuthority())
	{
		if (!IsActorBeingDestroyed())
		{
			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
			bool bHitPlayer = BlasterCharacter ? true : false;
			Multicast_OnHit(bHitPlayer);
		}

		AController* FiringController = FiringPawn->GetController();
		if (FiringController)
		{

			TArray<AActor*> IgnoredActors;
			IgnoredActors.Add(FiringPawn);
			UGameplayStatics::ApplyRadialDamageWithFalloff(
				this,
				Damage,
				5.0f,
				GetActorLocation(),
				InnerDamageRadius,
				OuterDamageRadius,
				1.0f,
				UDamageType::StaticClass(),
				IgnoredActors,
				this,
				FiringController
			);
		}

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

		TArray<AActor*> OutActors;
		float SphereRadius = (InnerDamageRadius + OuterDamageRadius) / 2.0f;
		UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetActorLocation(), SphereRadius, ObjectTypes, ABlasterCharacter::StaticClass(), TArray<AActor*>(), OutActors);
		for (AActor* Actor : OutActors)
		{
			ABlasterCharacter* Character = Cast<ABlasterCharacter>(Actor);
			if (Character)
			{
				FVector Direction = (Character->GetActorLocation() - GetActorLocation()).GetSafeNormal();
				FVector LaunchVelocity = (Direction * LaunchMagnitude) * FVector(1.0f, 1.0f, ZLaunchMultiplier);
				Character->LaunchCharacter(LaunchVelocity, true, false);
			}
		}
	}
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::Destroyed()
{
	Super::Destroyed();

}

