// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LoadingGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BEYONDPORTAL_API ALoadingGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ALoadingGameMode();

protected:
	virtual void BeginPlay() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> LoadingUIClass;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UUserWidget> LoadingUI;

};
