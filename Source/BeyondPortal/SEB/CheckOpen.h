// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CheckOpen.generated.h"

UCLASS()
class BEYONDPORTAL_API ACheckOpen : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckOpen();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* checkOpen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMaterial* mat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isChange=false;

};
