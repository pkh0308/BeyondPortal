// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SEB/LasersParent.h"
#include "Laser.generated.h"

/**
 * 
 */
UCLASS()
class BEYONDPORTAL_API ALaser : public ALasersParent
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
public:

	void ChangeColor();

	void SetEndPoint();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float L_Range = 3000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHitResult L_HitInfo;

	AActor* L_HitActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterial* laserColorMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UInterface> laserInterface;

	class ILasers* laserI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isDI;


	AActor* LastHitActor;

	UPROPERTY(EditInstanceOnly)
	FString Laser_ID;

	FString UpdateId(FString P_Append);
};
