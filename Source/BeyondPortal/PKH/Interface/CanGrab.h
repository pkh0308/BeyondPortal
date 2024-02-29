// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CanGrab.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCanGrab : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BEYONDPORTAL_API ICanGrab
{
	GENERATED_BODY()

public:
	virtual void Grab(ACharacter* NewOwner)=0;
	virtual void Drop()=0;
};
