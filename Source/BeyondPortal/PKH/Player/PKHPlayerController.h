// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PKHPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BEYONDPORTAL_API APKHPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	APKHPlayerController();

protected:
	virtual void BeginPlay() override;

// InputMode
protected:
	FInputModeGameOnly GameMode;
	FInputModeUIOnly UIMode;
};
