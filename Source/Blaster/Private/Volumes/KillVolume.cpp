// Fill out your copyright notice in the Description page of Project Settings.


#include "Volumes/KillVolume.h"
#include "Components/BoxComponent.h"
#include "Interfaces/OutOfBoundsInterface.h"


AKillVolume::AKillVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	Volume = CreateDefaultSubobject<UBoxComponent>(TEXT("Volume"));
	SetRootComponent(Volume);
}

void AKillVolume::BeginPlay()
{
	Super::BeginPlay();
	
	Volume->OnComponentBeginOverlap.AddDynamic(this, &AKillVolume::OnVolumeBeginOverlap);
}

void AKillVolume::OnVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UOutOfBoundsInterface>())
	{
		IOutOfBoundsInterface* Interface = Cast<IOutOfBoundsInterface>(OtherActor);
		if (Interface)
		{
			Interface->HandleOutOfBounds();
		}
	}
}


