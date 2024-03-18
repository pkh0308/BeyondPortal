// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadingUIWidget.generated.h"

/**
 * 
 */
UCLASS()
class BEYONDPORTAL_API ULoadingUIWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(EditDefaultsOnly, meta=(BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> TextAnim;
};
