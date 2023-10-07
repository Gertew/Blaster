// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HUD/BlasterHUD.h"
#include "Weapon/WeaponTypes.h"
#include "BlasterTypes/CombatState.h"
#include "CombatComponent.generated.h"

#pragma region Forward Declarations
class ABlasterCharacter;
class AWeapon;
class ABlasterPlayerController;
class ABlasterHUD;
class AProjectile;
#pragma endregion Forward Declarations

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	friend ABlasterCharacter;
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

#pragma region Combat Mechanics
	void EquipWeapon(AWeapon* WeaponToEquip);
	void SwitchWeapons();
	void Fire(bool bPressed);
	void SetWalkSpeed(float NewWalkSpeed);
	void SetAiming(bool bIsAiming);
#pragma endregion Combat Mechanics

#pragma region Reload Weapon
	void Reload();
	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	UFUNCTION(Server, Reliable)
	void ServerFinishReloading();

	UFUNCTION(BlueprintCallable)
	void FinishSwap();

	UFUNCTION(Server, Reliable)
	void ServerFinishSwap();

	UFUNCTION(BlueprintCallable)
	void FinishSwapAttachWeapons();

	UFUNCTION(BlueprintCallable)
	void ShotgunShellReload();
	void JumpToShotgunEnd();

	void PickupAmmo(EWeaponType WeaponType, int32 AmmoAmount);

	bool bLocallyReloading = false;

#pragma endregion Reload Weapon

#pragma region Grenade
	UFUNCTION(BlueprintCallable)
	void ThrowGrenadeFinished();

	UFUNCTION(Server, Reliable)
	void ServerThrowGrenadeFinished();

	UFUNCTION(BlueprintCallable)
	void LaunchGrenade();

	UFUNCTION(Server, Reliable)
	void ServerLaunchGrenade(const FVector_NetQuantize& Target);
#pragma endregion Grenade

protected:
	virtual void BeginPlay() override;

	void TraceUnderCrosshair(FHitResult& TraceHitResult);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

#pragma region Firing Weapon
	void ExecuteFire();
	void FireProjectileWeapon();
	void FireHitScanWeapon();
	void FireShotgun();
	void LocalFire(const FVector_NetQuantize& TraceHitTarget);
	void LocalShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTargets);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget, float FireDelay);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTargets, float FireDelay);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTargets);

#pragma endregion Firing Weapon

#pragma region Equipping Weapon
	void EquipPrimaryWeapon(AWeapon* WeaponToEquip);
	void EquipSecondaryWeapon(AWeapon* WeaponToEquip);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	UFUNCTION()
	void OnRep_SecondaryWeapon();

	bool bCanSwitchWeapons = false;
	FTimerHandle SwitchWeaponsTimer;
	float SwitchWeaponsCooldownTime = 1.0f;
	void StartSwitchWeaponsCooldownTimer();
	void OnSwitchWeaponsTimerFinish();

	void SetHUDCrosshair(float DeltaTime);
	void PlayEquipWeaponSound(AWeapon* WeaponToEquip);
	void UpdateCarriedAmmo();

	void DropEquippedWeapon();
	void AttachActorToRightHand(AActor* ActorToAttach);
	void AttachActorToLeftHand(AActor* ActorToAttach);
	void AttachActorToBackpack(AActor* ActorToAttach);
	void AttachFlagToLeftHand(AActor* Flag);
#pragma endregion Equipping Weapon

#pragma region Reloading Weapon
	UFUNCTION(Server, Reliable)
	void ServerReload();

	void HandleReload();

	int32 AmountToReload();

	void ReloadEmptyWeapon();

#pragma endregion Reloading Weapon

#pragma region Grenade
	void ThrowGrenade();
	UFUNCTION(Server, Reliable)
	void ServerThrowGrenade();

	void ShowAttachedGrenade(bool bShowGrenade);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AProjectile> GrenadeClass;

#pragma endregion Grenade

private:
#pragma region Combat Internals
	UPROPERTY()
	TObjectPtr<ABlasterCharacter> Character;
	UPROPERTY()
	TObjectPtr<ABlasterPlayerController> PlayerController;
	UPROPERTY()
	TObjectPtr<ABlasterHUD> HUD;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	TObjectPtr<AWeapon> EquippedWeapon;

	UPROPERTY(ReplicatedUsing = OnRep_SecondaryWeapon)
	TObjectPtr<AWeapon> SecondaryWeapon;

	UPROPERTY(ReplicatedUsing = OnRep_Aiming)
	bool bAiming = false;

	UFUNCTION()
	void OnRep_Aiming();

	bool bAimActive = false;

	bool bFired;

	FVector HitTarget;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;

	UFUNCTION()
	void OnRep_CombatState();
#pragma endregion Combat Internals

#pragma region Movement Speed
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WalkSpeed", meta = (AllowPrivateAccess = "true"))
	float BaseWalkSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WalkSpeed", meta = (AllowPrivateAccess = "true"))
	float AimWalkSpeed;
#pragma endregion Movement Speed

#pragma region Firing Error
	FHUDPackage HUDPackage;
	float CrosshairVelocityFactor;
	float CrosshairFallingFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;
	UPROPERTY(EditDefaultsOnly)
	float CrosshairAimedFactor = -0.175f;
#pragma endregion Firing Error

#pragma region Field of View
	float CurrentFOV;
	float DefaultFOV;
	UPROPERTY(EditDefaultsOnly)
	float ZoomedFOV = 30.0f;
	UPROPERTY(EditDefaultsOnly)
	float ZoomInterpSpeed = 20.0f;

	void InterpFOV(float DeltaTime);
#pragma endregion Field of View

#pragma region Firing Weapon
	bool bCanFire = true;
	bool CanFire();
	FTimerHandle FireTimer;
	void StartFireTimer();
	void FireTimerFinished();
#pragma endregion Firing Weapon

#pragma region Carried Ammo
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	UFUNCTION()
	void OnRep_CarriedAmmo();

	TMap<EWeaponType, int32> CarriedAmmoMap;
	void InitializeCarriedAmmo();
#pragma endregion Carried Ammo

#pragma region Starting and Max Ammos
	UPROPERTY(EditDefaultsOnly)
	int32 StartingARAmmo = 0;
	UPROPERTY(EditDefaultsOnly)
	int32 StartingRocketAmmo = 0;
	UPROPERTY(EditDefaultsOnly)
	int32 StartingPistolAmmo = 0;
	UPROPERTY(EditDefaultsOnly)
	int32 StartingSMGAmmo = 0;
	UPROPERTY(EditDefaultsOnly)
	int32 StartingShotgunAmmo = 0;
	UPROPERTY(EditDefaultsOnly)
	int32 StartingSniperAmmo = 0;
	UPROPERTY(EditDefaultsOnly)
	int32 StartingGrenadeLauncherAmmo = 0;
	UPROPERTY(EditDefaultsOnly)
	int32 StartingSpecialAmmo = 0;
	UPROPERTY(EditDefaultsOnly)
	int32 MaxAmmoAmount = 99;

	UPROPERTY(EditDefaultsOnly)
	int32 StartingGrenades = 2;
	UPROPERTY(ReplicatedUsing = OnRep_Grenades)
	int32 Grenades;
	UPROPERTY(EditDefaultsOnly)
	int32 MaxGrenades = 4;

	UFUNCTION()
	void OnRep_Grenades();
#pragma endregion Starting and Max Ammos

#pragma region Reloading
	void UpdateHUDGrenades();
	void UpdateAmmoValues();
	void UpdateShotgunAmmoValues();
#pragma endregion Reloading

#pragma region Capture The Flag
	UPROPERTY(ReplicatedUsing = OnRep_HoldingFlag)
	bool bHoldingFlag = false;

	UFUNCTION()
	void OnRep_HoldingFlag();

	UPROPERTY()
	AWeapon* TheFlag;
#pragma endregion Capture The Flag

public:	
#pragma region Getters/Setters
	FORCEINLINE int32 GetGrenades() const { return Grenades; }
	int32 GetEquippedWeaponAmmo();
	FORCEINLINE int32 GetCarriedAmmo() const { return CarriedAmmo; }
	UTexture2D* GetEquippedWeaponIcon();
	bool ShouldSwapWeapons();
#pragma endregion Getters/Setters
};
