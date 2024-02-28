// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PKH/Interface/Interactible.h"
#include "GrabCube.generated.h"

UCLASS()
class BEYONDPORTAL_API AGrabCube : public AActor, public IInteractible
{
	GENERATED_BODY()
	
public:	
	AGrabCube();

protected:
	virtual void BeginPlay() override;

// Component
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UBoxComponent> BoxComp;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UStaticMeshComponent> MeshComp;

// Interface
public:
	virtual void DoInteraction() override;
};
