﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "GameFramework/Actor.h"
#include "FloorLine.generated.h"

UENUM()
enum class ESplineMeshType: uint8
{
	DEFAULT UMETA(DisplayName = "Default Mesh"),
	START UMETA(DisplayName = "Starting Mesh"),
	END UMETA(DisplayName = "Ending Mesh"),
};
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
	class UStaticMeshComponent* lineMeshComp;

	
	UFUNCTION()
	void ChangeOrange(AActor* changeLine);

};
