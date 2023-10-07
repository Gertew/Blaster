// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "BlasterTypes/TurningInPlace.h"
#include "Interfaces/CrosshairInteractInterface.h"
#include "Components/TimelineComponent.h"
#include "BlasterTypes/CombatState.h"
#include "Interfaces/OutOfBoundsInterface.h"
#include "BlasterTypes/Team.h"
#include "BlasterCharacter.generated.h"

#pragma region Forward Declarations
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UWidgetComponent;
class AWeapon;
class UCombatComponent;
class UAnimMontage;
class ABlasterPlayerController;
class USoundCue;
class ABlasterPlayerState;
class UBuffComponent;
class UBoxComponent;
class ULagCompensationComponent;
class UNiagaraSystem;
class UNiagaraComponent;
class ABlasterGameMode;
#pragma endregion Forward Declarations

#pragma region Custom Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeftGame);
#pragma endregion Custom Delegates

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter, public ICrosshairInteractInterface, public IOutOfBoundsInterface
{
	GENERATED_BODY()

public:
	ABlasterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual void Destroyed() override;
	void SpawnDefaultWeapon();

#pragma region Out of Bounds Interface
	virtual void HandleOutOfBounds() override;
#pragma endregion Out of Bounds Interface

#pragma region Play Montages
	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();
	void PlayEliminatedMontage();
	void PlayThrowGrenadeMontage();
	void PlaySwapWeaponsMontage();
#pragma endregion Play Montages

#pragma region Local Cosmetics
	virtual void OnRep_ReplicatedMovement() override;

	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScopeWidget(bool bShowScope);

	bool bFinishedSwapping = false;

#pragma endregion Local Cosmetics

#pragma region Player Eliminated
	void Eliminated(bool bPlayerLeftGame);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastEliminated(bool bPlayerLeftGame);

	UPROPERTY(Replicated)
	bool bDisableGameplay = false;
#pragma endregion Player Eliminated

#pragma region HUD
	void UpdateHUDHealth();
	void UpdateHUDShield();
#pragma endregion HUD

#pragma region Server Side Rewind
	TMap<FName, UBoxComponent*> HitCollisionBoxes;
#pragma endregion Server Side Rewind

#pragma region Leave Game
	FOnLeftGame OnLeftGame;

	UFUNCTION(Server, Reliable)
	void ServerLeaveGame();
#pragma endregion Leave Game

#pragma region Score Leaderboard
	UFUNCTION(NetMulticast, Reliable)
	void MulticastGainedTheLead();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastLostTheLead();
#pragma endregion Score Leaderboard

#pragma region Team
	void SetTeamColor(ETeam Team);
#pragma endregion Team

protected:
	virtual void BeginPlay() override;

#pragma region Player Setup
	void OnPlayerStateInitialized();
	void SetSpawnPoint();
	void PollInit();
#pragma endregion Player Setup

#pragma region Character Actions
	virtual void Jump() override;
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void TryEquip();
	void TryCrouch();
	void TryAim();
	void TryUnaim();
	void TryFire();
	void TryStopFire();
	void TryReload();
	void TryThrowGrenade();
	void TrySwitchWeapons();
#pragma endregion Character Actions

#pragma region Enhanced Input
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputMappingContext> BlasterContext;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> JumpAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> EquipAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> CrouchAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> AimAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> FireAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> ReloadAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> ThrowGrenadeAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> SwitchWeaponAction;
#pragma endregion Enhanced Input

#pragma region Simulated Proxies
	void AimOffset(float DeltaTime);
	void CalculateAO_Pitch();
	void SimProxiesTurn();
	void RotateInPlace(float DeltaTime);
#pragma endregion Simulated Proxies

#pragma region HUD
	void HideHUDElim();
	void UpdateHUDAmmo();
#pragma endregion HUD

#pragma region Weapon
	void DropOrDestroyWeapons();
	void DropOrDestroyWeapon(AWeapon* Weapon);
#pragma endregion Weapon

#pragma region Damage
	UFUNCTION()	
	void ReceiveDamage(AActor* DamageActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);
	void PlayHitReactMontage();
#pragma endregion Damage

private:
#pragma region Server Side Rewind Hitboxes
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> head;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> pelvis;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> spine_02;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> spine_03;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> upperarm_l;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> upperarm_r;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> lowerarm_l;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> lowerarm_r;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> hand_l;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> hand_r;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> backpack;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> blanket;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> thigh_l;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> thigh_r;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> calf_l;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> calf_r;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> foot_l;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> foot_r;
#pragma endregion Server Side Rewind Hitboxes

#pragma region Components
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArm;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Widget, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> OverheadWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCombatComponent> CombatComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBuffComponent> BuffComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ULagCompensationComponent> LagCompensationComponent;
#pragma endregion Components

#pragma region Overlapping Weapon
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	TObjectPtr<AWeapon> OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);
#pragma endregion Overlapping Weapon

#pragma region Server Requests
	UFUNCTION(Server, Reliable)
	void ServerTryEquip();

	UFUNCTION(Server, Reliable)
	void ServerTrySwitchWeapons();
#pragma endregion Server Requests

#pragma region Simulated Proxies Turn
	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);

	bool bRotateRootBone;
	float TurnThreshold = 0.5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;

	float CalculateSpeed();
#pragma endregion Simulated Proxies Turn

#pragma region Montages
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> FireWeaponMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> ReloadWeaponMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> EliminatedMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> ThrowGrenadeMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> SwapWeaponsMontage;
#pragma endregion Montages

#pragma region Occlusion
	void TryOccludeCharacter();
	UPROPERTY(EditDefaultsOnly)
	float CameraThreshold = 200.0f;
#pragma endregion Occlusion

#pragma region Character Attributes
	UPROPERTY(EditDefaultsOnly, Category = "Player State")
	float MaxHealth = 100.0f;
	UPROPERTY(ReplicatedUsing = OnRep_Health, EditDefaultsOnly, Category = "Player State")
	float Health = 100.0f;

	UFUNCTION()
	void OnRep_Health(float LastHealth);

	UPROPERTY(EditDefaultsOnly, Category = "Player State")
	float MaxShield = 100.0f;
	UPROPERTY(ReplicatedUsing = OnRep_Shield, EditDefaultsOnly, Category = "Player State")
	float Shield = 0.0f;

	UFUNCTION()
	void OnRep_Shield(float LastShield);
#pragma endregion Player Attributes

#pragma region Player Elimination
	bool bEliminated;

	FTimerHandle EliminatedTimer;
	UPROPERTY(EditDefaultsOnly, Category = "Player State")
	float EliminatedDelay = 3.0f;
	void EliminatedTimerFinished();

	bool bLeftGame = false;

	UPROPERTY(EditDefaultsOnly, Category = Elimination)
	TObjectPtr<UCurveFloat> DissolveCurve;
	UPROPERTY(EditDefaultsOnly, Category = Elimination)
	TObjectPtr<UMaterialInstance> MI_Dissolve;
	UPROPERTY(VisibleDefaultsOnly, Category = Elimination)
	TObjectPtr<UMaterialInstanceDynamic> MID_Dissolve;
	UPROPERTY(VisibleDefaultsOnly, Category = Elimination)
	TObjectPtr<UTimelineComponent> DissolveTimeline;
	FOnTimelineFloat DissolveTrack;
	void StartDissolve();
	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);

	UPROPERTY(EditDefaultsOnly, Category = Elimination)
	TObjectPtr<UMaterialInstance> MI_Dissolve_Red;
	UPROPERTY(EditDefaultsOnly, Category = Elimination)
	TObjectPtr<UMaterialInstance> MI_Dissolve_Blue;

	UPROPERTY(EditDefaultsOnly, Category = Elimination)
	TObjectPtr<UMaterialInstance> MI_Red;
	UPROPERTY(EditDefaultsOnly, Category = Elimination)
	TObjectPtr<UMaterialInstance> MI_Blue;

	UPROPERTY(EditDefaultsOnly, Category = Elimination)
	TObjectPtr<UMaterialInstance> MI_Default;

	UPROPERTY(EditDefaultsOnly, Category = Elimination)
	TObjectPtr<UParticleSystem> EliminationEffect;
	UPROPERTY(VisibleDefaultsOnly, Category = Elimination)
	TObjectPtr<UParticleSystemComponent> EliminationEffectComponent;
	UPROPERTY(EditDefaultsOnly, Category = Elimination)
	TObjectPtr<USoundCue> EliminationSound;
#pragma endregion Player Elimination

#pragma region Player Score
	UPROPERTY(EditDefaultsOnly, Category = Elimination)
	TObjectPtr<UNiagaraSystem> CrownSystem;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> CrownComponent;
#pragma endregion Player Score

#pragma region Character Internals
	UPROPERTY()
	TObjectPtr<ABlasterPlayerController> BlasterPlayerController;

	UPROPERTY()
	TObjectPtr<ABlasterPlayerState> BlasterPlayerState;

	UPROPERTY()
	TObjectPtr<ABlasterGameMode> BlasterGameMode;
#pragma endregion Character Internals

#pragma region Grenades
	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> AttachedGrenade;
#pragma endregion Grenades

#pragma region Weapons
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AWeapon> DefaultWeaponClass;
#pragma endregion Weapons

public:
#pragma region Getters/Setters
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquiped();
	bool IsAiming();
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	AWeapon* GetEquippedWeapon();
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FVector GetHitTarget() const;
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE bool IsEliminated() const { return bEliminated; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE void SetHealth(float Amount) { Health = Amount; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetShield() const { return Shield; }
	FORCEINLINE void SetShield(float Amount) { Shield = Amount; }
	FORCEINLINE float GetMaxShield() const { return MaxShield; }
	ECombatState GetCombatState() const;
	FORCEINLINE UCombatComponent* GetCombatComponent() const { return CombatComponent; }
	FORCEINLINE bool GetDisableGameplay() const { return bDisableGameplay; }
	FORCEINLINE UInputAction* GetLookAction() const { return LookAction; }
	FORCEINLINE UAnimMontage* GetReloadMontage() const { return ReloadWeaponMontage; }
	FORCEINLINE UStaticMeshComponent* GetAttachedGrenade() const { return AttachedGrenade; }
	FORCEINLINE UBuffComponent* GetBuffComponent() const { return BuffComponent; }
	FORCEINLINE ABlasterPlayerState* GetBlasterPlayerState() const { return BlasterPlayerState; }
	bool IsLocallyReloading();
	FORCEINLINE ULagCompensationComponent* GetLagCompensation() const { return LagCompensationComponent; }
	FORCEINLINE bool IsHoldingFlag() const;
	UFUNCTION(BlueprintPure)
	ETeam GetTeam();
	void SetHoldingFlag(bool bHolding);
#pragma endregion Getters/Setters

};
