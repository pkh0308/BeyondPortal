// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnCube.generated.h"

UCLASS()
class BEYONDPORTAL_API ASpawnCube : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnCube();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSubclassOf<AActor> spawnCube;

	void Spawn();

	UFUNCTION(Server, Reliable)
	void RPC_Server_Spawn();

	TArray<AActor*> BarrierButtonCheck;

	bool isSpawned=false;

};
