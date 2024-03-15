// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyUIWidget.generated.h"

DECLARE_DELEGATE_TwoParams(FOnFindRoomCompleteDelegate, bool, class FOnlineSessionSearchResult* /*bExist*/)

/**
 * 
 */
UCLASS()
class BEYONDPORTAL_API ULobbyUIWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

// Game Instance
protected:
	UPROPERTY()
	TObjectPtr<class UPortalGameInstance> GameInst;

// Widgets
protected:
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<class UButton> Btn_StartGame;

	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<class UCanvasPanel> PopUp_WaitPlayer;

// Bind
protected:
	UFUNCTION()
	void OnClicked_StartGame();

// Delegate
public:
	FOnFindRoomCompleteDelegate OnFindRoomComplete;

	void DoesRoomExist(bool bExist, class FOnlineSessionSearchResult* result);
};
