// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponTypes.h"
#include "Interfaces/OutOfBoundsInterface.h"
#include "BlasterTypes/Team.h"
#include "Weapon.generated.h"

#pragma region Forward Declarations
class USphereComponent;
class UWidgetComponent;
class UAnimationAsset;
class ACasing;
class UTexture2D;
class ABlasterCharacter;
class ABlasterPlayerController;
class USoundCue;
#pragma endregion Forward Declarations

#pragma region Weapon Enums
UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "InitialState"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_EquippedSecondary UMETA(DisplayName = "Equipped Secondary"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),
	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EFireType : uint8
{
	EFT_HitScan UMETA(DisplayName = "HitScan Weapon"),
	EFT_Projectile UMETA(DisplayName = "Projectile Weapon"),
	EFT_Shotgun UMETA(DisplayName = "Shotgun Weapon"),
	EFT_MAX UMETA(DisplayName = "DefaultMax")
};
#pragma endregion Weapon Enums

UCLASS()
class BLASTER_API AWeapon : public AActor, public IOutOfBoundsInterface
{
	GENERATED_BODY()
	
public:	
	AWeapon();

#pragma region Out of Bounds Interface
	virtual void HandleOutOfBounds() override;
#pragma endregion Out of Bounds Interface

	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

#pragma region Cosmetics
	void ShowPickupWidget(bool bShowWidget);
	void EnableCustomDepth(bool bEnable);
	void SetHUDAmmo();
	void SetHUDIcon();
#pragma endregion Cosmetics

#pragma region Gameplay
	virtual void Fire(const FVector& HitTarget);
	virtual void Dropped();
	virtual void OnRep_Owner();
	void AddAmmo(int32 AmmoToAdd);
	FVector TraceEndWithScatter(const FVector& HitTarget);
#pragma endregion Gameplay

#pragma region Crosshair
	UPROPERTY(EditDefaultsOnly, Category = Crosshair)
	TObjectPtr<UTexture2D> CrosshairCenter;
	UPROPERTY(EditDefaultsOnly, Category = Crosshair)
	TObjectPtr<UTexture2D> CrosshairLeft;
	UPROPERTY(EditDefaultsOnly, Category = Crosshair)
	TObjectPtr<UTexture2D> CrosshairRight;
	UPROPERTY(EditDefaultsOnly, Category = Crosshair)
	TObjectPtr<UTexture2D> CrosshairTop;
	UPROPERTY(EditDefaultsOnly, Category = Crosshair)
	TObjectPtr<UTexture2D> CrosshairBottom;
#pragma endregion Crosshair

	bool bDestroyWeapon = false;

protected:
	virtual void BeginPlay() override;

#pragma region Protected Gameplay
	virtual void OnWeaponStateSet();
	virtual void OnEquipped();
	virtual void OnEquippedSecondary();
	virtual void OnDropped();

	UFUNCTION()
	void OnPingTooHigh(bool bPingTooHigh);
#pragma endregion Protected Gameplay

#pragma region Area Sphere Callbacks
	UFUNCTION()
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
#pragma endregion Area Sphere Callbacks

#pragma region Weapon Scatter
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Scatter")
	bool bUseScatter;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Scatter")
	float DistanceToSphere = 800.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Scatter")
	float SphereRadius = 75.0f;
#pragma endregion Weapon Scatter

#pragma region Protected Weapon Properties
	UPROPERTY(EditDefaultsOnly, Replicated, Category = "Server Side Rewind")
	bool bUseServerSideRewind = false;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Properties")
	float Damage = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Properties")
	float HeadshotDamageMultiplier = 1.0f;
#pragma endregion Protected Weapon Properties

#pragma region Owner
	UPROPERTY()
	TObjectPtr<ABlasterCharacter> BlasterOwnerCharacter;

	UPROPERTY()
	TObjectPtr<ABlasterPlayerController> BlasterOwnerPlayerController;
#pragma endregion Owner

private:
	UFUNCTION()
	void OnRep_WeaponState();

#pragma region Client Side Ammo Prediction
	void SpendAmmo(int32 Amount);

	UFUNCTION(Server, Reliable)
	void ClientUpdateAmmo(int32 ServerAmmo);

	UFUNCTION(NetMulticast, Reliable)
	void ClientAddAmmo(int32 AmmoToAdd);
#pragma endregion Client Side Ammo Predicition

#pragma region Components
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USphereComponent> AreaSphere;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UWidgetComponent> PickupWidget;
#pragma endregion Components

#pragma region Weapon Properties
	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Properties")
	ETeam Team;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Properties")
	EWeaponType WeaponType;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Properties")
	EFireType FireType;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Properties")
	bool bAutomatic = true;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Properties")
	float FireDelay = 0.15f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Properties")
	int32 Ammo;

	// Unprocessed Server Requests For Ammo
	int32 Sequence = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Properties")
	int32 MagCapacity;

	FVector InitialLocation;
#pragma endregion Weapon Properties

#pragma region Weapon Cosmetics/Effects
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Effects")
	TObjectPtr<UAnimationAsset> FireAnimation;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Effects")
	TSubclassOf<ACasing> CasingClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Effects")
	TObjectPtr<USoundCue> EquipSound;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Effects")
	TObjectPtr<UTexture2D> WeaponIcon;
#pragma endregion Weapon Cosmetics/Effects

#pragma region Weapon Aiming
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Aiming")
	float ZoomedFOV = 30.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Aiming")
	float ZoomInterpSpeed = 20.0f;
#pragma endregion Weapon Aiming

public:	
#pragma region Getters/Setters
	void SetWeaponState(EWeaponState State);
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }
	bool IsEmpty();
	bool IsFull();
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; }
	FORCEINLINE bool GetMagFull() const { return Ammo == MagCapacity; }
	FORCEINLINE UTexture2D* GetWeaponIcon() const { return WeaponIcon; }
	FORCEINLINE FVector GetInitialLocation() const { return InitialLocation; }
	FORCEINLINE bool GetUseScatter() const { return bUseScatter; }
	FORCEINLINE void SetUseScatter(bool State) { bUseScatter = State; }
	FORCEINLINE float GetDamage() const { return Damage; }
	FORCEINLINE float GetHeadshotDamage() const { return Damage * HeadshotDamageMultiplier; }
	FORCEINLINE ETeam GetTeam() const { return Team; }
	FORCEINLINE USoundCue* GetEquipSound() const { return EquipSound; }
	FORCEINLINE bool GetIsAutomatic() const { return bAutomatic; }
	FORCEINLINE float GetFireDelay() const { return FireDelay; }
	FORCEINLINE EFireType GetFireType() const { return FireType; }

#pragma endregion Getters/Setters
};
