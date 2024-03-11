// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PKHGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BEYONDPORTAL_API APKHGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APKHGameMode();

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class APlayerCharacter> P2CharacterClass;

	UPROPERTY(VisibleAnywhere)
	int LogInCount=0;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	
};
