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

// Count
protected:
	UPROPERTY()
	TMap<int32, int32> CountMap;

public:
	void AddCount(int32 Key, int32 Value = 1);
	int32 GetCount(int32 Key);
};
