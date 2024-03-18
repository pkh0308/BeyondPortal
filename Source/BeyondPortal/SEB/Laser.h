// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Laser.generated.h"

UCLASS()
class BEYONDPORTAL_API ALaser : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALaser();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* LaserEmitter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* Cube;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UArrowComponent* Arrow;


	//-------------------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CastDistance=100000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> Beams;

	void CastLight(FVector CastOrigin, FVector CastDirection, float CastDistances);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* mat;
};
