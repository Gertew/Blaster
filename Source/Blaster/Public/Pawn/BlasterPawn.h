// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BlasterPawn.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class BLASTER_API ABlasterPawn : public APawn
{
	GENERATED_BODY()

public:
	ABlasterPawn();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = Camera)
	TObjectPtr<USpringArmComponent> SpringArm;
	UPROPERTY(EditDefaultsOnly, Category = Camera)
	TObjectPtr<UCameraComponent> Camera;

public:	


};
