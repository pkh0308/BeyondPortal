// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GunActor.generated.h"

UCLASS()
class BEYONDPORTAL_API AGunActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AGunActor();

protected:
	virtual void BeginPlay() override;

// Components
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UBoxComponent> BoxComp;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UStaticMeshComponent> MeshComp;

public:
	void SetActive(bool Active);

};
