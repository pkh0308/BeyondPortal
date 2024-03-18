// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameClearUIWidget.generated.h"

/**
 * 
 */
UCLASS()
class BEYONDPORTAL_API UGameClearUIWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<class UTextBlock> Txt_PlayTime;

	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<class UTextBlock> Txt_PortalCount;

	UPROPERTY(EditDefaultsOnly, meta=(BindWidgetAnim), Transient)
	TObjectPtr<class UWidgetAnimation> ClearAnim;

public:
	void SetClearUI(int32 PlayTime, int32 PortalCount);
};
