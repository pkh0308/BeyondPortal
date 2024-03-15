// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LasersParent.generated.h"

UCLASS()
class BEYONDPORTAL_API ALasersParent : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALasersParent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* SM_Parent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Active=false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor LaserColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ALaserSpawn* SpawnReference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class ALaser*> LasersAttached;


	void ClearAllLasersAttached();
};
