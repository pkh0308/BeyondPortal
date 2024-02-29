// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PKH/Interface/CanGrab.h"
#include "GrabCube.generated.h"

UCLASS()
class BEYONDPORTAL_API AGrabCube : public AActor, public ICanGrab
{
	GENERATED_BODY()
	
public:	
	AGrabCube();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

// Component
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UBoxComponent> BoxComp;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UStaticMeshComponent> MeshComp;

// Interface
public:
	virtual void Grab(ACharacter* NewOwner) override;

	virtual void Drop() override;

// Grab
protected:
	UPROPERTY()
	TObjectPtr<class APlayerCharacter> OwnPlayer;

	void TickGrab();

};
