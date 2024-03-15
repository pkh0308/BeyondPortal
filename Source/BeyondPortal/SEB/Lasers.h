// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Laser.h"
#include "UObject/Interface.h"
#include "Lasers.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULasers : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BEYONDPORTAL_API ILasers
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual ALaser* LaserBounce(class ALaser* P_LaserHit, bool P_HitResponse, FHitResult P_HitInformation) = 0;
};
