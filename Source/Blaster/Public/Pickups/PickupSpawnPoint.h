// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupSpawnPoint.generated.h"

class APickup;

UCLASS()
class BLASTER_API APickupSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	APickupSpawnPoint();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	void SpawnPickup();
	UFUNCTION()
	void StartSpawnPickupTimer(AActor* DestroyedActor);
	void SpawnPickupTimerFinished();

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<APickup>> PickupClasses;

	UPROPERTY()
	TObjectPtr<APickup> SpawnedPickup;

private:
	UPROPERTY(EditAnywhere)
	float SpawnPickupTimeMin;
	UPROPERTY(EditAnywhere)
	float SpawnPickupTimeMax;

	FTimerHandle SpawnPickupTimer;

public:	


};
