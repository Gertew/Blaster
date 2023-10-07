// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BlasterAnimInstance.h"
#include "Character/BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon/Weapon.h"
#include "BlasterTypes/CombatState.h"

void UBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
	if (BlasterCharacter)
	{
		BlasterMovementComponent = BlasterCharacter->GetCharacterMovement();
	}
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (BlasterMovementComponent)
	{
		Velocity = BlasterMovementComponent->Velocity;
		GroundSpeed = UKismetMathLibrary::VSizeXY(Velocity);
		bIsFalling = BlasterMovementComponent->IsFalling();
		bIsAccelerating = BlasterMovementComponent->GetCurrentAcceleration().Size() > 0.5f ? true : false;
		bIsCrouched = BlasterMovementComponent->IsCrouching();
	}

	if (BlasterCharacter)
	{
		// Yaw Offset For Strafing
		FRotator AimRotation = BlasterCharacter->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(Velocity);
		FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
		DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 6.0f);
		YawOffset = DeltaRotation.Yaw;

		CharacterRotationLastFrame = CharacterRotation;
		CharacterRotation = BlasterCharacter->GetActorRotation();
		const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
		const float Target = Delta.Yaw / DeltaTime;
		const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.0f);
		Lean = FMath::Clamp(Interp, -90.0f, 90.0f);

		bWeaponEquipped = BlasterCharacter->IsWeaponEquiped();
		EquippedWeapon = BlasterCharacter->GetEquippedWeapon();
		bIsAiming = BlasterCharacter->IsAiming();
		TurningInPlace = BlasterCharacter->GetTurningInPlace();
		bRotateRootBone = BlasterCharacter->ShouldRotateRootBone();
		bEliminated = BlasterCharacter->IsEliminated();
		bHoldingFlag = BlasterCharacter->IsHoldingFlag();
		bUseFABRIK = BlasterCharacter->GetCombatState() == ECombatState::ECS_Unoccupied;
		bool bFABRIKOverride = BlasterCharacter->IsLocallyControlled() &&
			BlasterCharacter->GetCombatState() != ECombatState::ECS_ThrowingGrenade &&
			BlasterCharacter->bFinishedSwapping;
		if (bFABRIKOverride)
		{
			bUseFABRIK = !BlasterCharacter->IsLocallyReloading();
		}
		bUseAimOffsets = BlasterCharacter->GetCombatState() != ECombatState::ECS_Unoccupied && !BlasterCharacter->GetDisableGameplay();
		bTransformRightHand = BlasterCharacter->GetCombatState() != ECombatState::ECS_Unoccupied && !BlasterCharacter->GetDisableGameplay();

		AO_Yaw = BlasterCharacter->GetAO_Yaw();
		AO_Pitch = BlasterCharacter->GetAO_Pitch();

		if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && BlasterCharacter->GetMesh())
		{
			LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"));

			FVector OutPosition;
			FRotator OutRotation;
			BlasterCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);

			LeftHandTransform.SetLocation(OutPosition);
			LeftHandTransform.SetRotation(FQuat(OutRotation));

			if (BlasterCharacter->IsLocallyControlled())
			{
				bLocallyControlled = true;
				FTransform RightHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("hand_r"));
				FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(
					RightHandTransform.GetLocation(),
					RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - BlasterCharacter->GetHitTarget())
				);
				RightHandRotation = FMath::RInterpTo(RightHandRotation, LookAtRotation, DeltaTime, 30.0f);
			}
		}
	}
}
