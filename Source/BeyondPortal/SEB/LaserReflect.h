// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Lasers.h"
#include "SEB/LasersParent.h"
#include "LaserReflect.generated.h"

/**
 * 
 */
UCLASS()
class BEYONDPORTAL_API ALaserReflect : public ALasersParent, public ILasers
{
	GENERATED_BODY()

public:
	ALaserReflect();
	virtual void BeginPlay() override;
	virtual ALaser* LaserBounce(class ALaser* P_LaserHit, bool P_HitResponse, FHitResult P_HitInformation) override;

	void ClearAttachedLasers(FString P_ID_Clear);
};
