// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ArrowComponent.h"
#include "SEB/LasersParent.h"
#include "LaserSpawn.generated.h"

/**
 * 
 */
UCLASS()
class BEYONDPORTAL_API ALaserSpawn : public ALasersParent
{
	GENERATED_BODY()

public:
	ALaserSpawn();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UArrowComponent* arrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class ALaser*> ChildLasers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ALaser* SpawnLaser;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class TSubclassOf<ALaser> laserFactory;

	void SetSpawnReference(ALaserSpawn* NewSpawnReference);
};
