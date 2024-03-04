// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloorLine.generated.h"

UCLASS()
class BEYONDPORTAL_API AFloorLine : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloorLine();

	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USplineComponent* spline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UInstancedStaticMeshComponent* instancedStaticMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMesh* chain;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float offset=0;

};
