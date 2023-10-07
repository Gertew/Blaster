// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Shotgun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "BlasterComponents/LagCompensationComponent.h"
#include "PlayerController/BlasterPlayerController.h"

void AShotgun::FireShotgun(const TArray<FVector_NetQuantize>& HitTargets)
{
	AWeapon::Fire(FVector());

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr)
	{
		return;
	}
	AController* InstigatorController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket)
	{
		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		const FVector Start = SocketTransform.GetLocation();

		TMap<ABlasterCharacter*, uint32> HitMap;
		TMap<ABlasterCharacter*, uint32> HeadshotHitMap;
		for (FVector_NetQuantize HitTarget : HitTargets)
		{
			FHitResult FireHit;
			WeaponTraceHit(Start, HitTarget, FireHit);

			ABlasterCharacter* HitCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
			if (HitCharacter)
			{
				const bool bHeadshot = FireHit.BoneName.ToString() == FString("head");

				if (bHeadshot)
				{
					if (HeadshotHitMap.Contains(HitCharacter))
					{
						HeadshotHitMap[HitCharacter]++;
					}
					else
					{
						HeadshotHitMap.Emplace(HitCharacter, 1);
					}
				}
				else
				{
					if (HitMap.Contains(HitCharacter))
					{
						HitMap[HitCharacter]++;
					}
					else
					{
						HitMap.Emplace(HitCharacter, 1);
					}
				}

				SpawnImpactEffects(FireHit);
			}
		}

		TArray<ABlasterCharacter*> HitCharacters;

		TMap<ABlasterCharacter*, float> DamageMap;
		for (TPair<ABlasterCharacter*, uint32> HitPair : HitMap)
		{
			if (HitPair.Key)
			{
				DamageMap.Emplace(HitPair.Key, HitPair.Value * Damage);

				HitCharacters.AddUnique(HitPair.Key);
			}
		}

		for (TPair<ABlasterCharacter*, uint32> HeadshotHitPair : HeadshotHitMap)
		{
			if (HeadshotHitPair.Key)
			{
				if (DamageMap.Contains(HeadshotHitPair.Key))
				{
					DamageMap[HeadshotHitPair.Key] += HeadshotHitPair.Value * GetHeadshotDamage();
				}
				else
				{
					DamageMap.Emplace(HeadshotHitPair.Key, HeadshotHitPair.Value * GetHeadshotDamage());
				}

				HitCharacters.AddUnique(HeadshotHitPair.Key);
			}
		}

		for (TPair<ABlasterCharacter*, float> DamagePair : DamageMap)
		{
			if (DamagePair.Key && InstigatorController)
			{
				bool bCauseAuthoritativeDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
				if (HasAuthority() && bCauseAuthoritativeDamage)
				{
					UGameplayStatics::ApplyDamage(DamagePair.Key, DamagePair.Value, InstigatorController, this, UDamageType::StaticClass());
				}
			}
		}

		if (!HasAuthority() && bUseServerSideRewind)
		{
			BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(OwnerPawn) : BlasterOwnerCharacter;
			BlasterOwnerPlayerController = BlasterOwnerPlayerController == nullptr ? Cast<ABlasterPlayerController>(InstigatorController) : BlasterOwnerPlayerController;
			if (BlasterOwnerPlayerController && BlasterOwnerCharacter && BlasterOwnerCharacter->GetLagCompensation() && BlasterOwnerCharacter->IsLocallyControlled())
			{
				BlasterOwnerCharacter->GetLagCompensation()->ServerShotgunScoreRequest(
					HitCharacters,
					Start,
					HitTargets,
					BlasterOwnerPlayerController->GetServerTime() - BlasterOwnerPlayerController->SingleTripTime
					);
			}
		}
	}
}

void AShotgun::ShotgunTraceEndWithScatter(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets)
{
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket == nullptr)
	{
		return;
	}

	const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
	const FVector TraceStart = SocketTransform.GetLocation();

	const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;

	for (uint32 i = 0; i < NumberOfPellets; i++)
	{
		const FVector RandomVector = UKismetMathLibrary::RandomUnitVector() * FMath::RandRange(0.0f, SphereRadius);
		const FVector EndLocation = SphereCenter + RandomVector;
		const FVector ToEndLocation = EndLocation - TraceStart;
		const FVector FinalLocation = TraceStart + ToEndLocation * TRACE_LENGTH / ToEndLocation.Size();
		HitTargets.Add(FinalLocation);
	}
}
