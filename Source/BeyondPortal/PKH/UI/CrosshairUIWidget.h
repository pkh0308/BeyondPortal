// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CrosshairUIWidget.generated.h"

/**
 * 
 */
UCLASS()
class BEYONDPORTAL_API UCrosshairUIWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UCrosshairUIWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

// Images
protected:
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<class UImage> Crosshair_Center;

	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<class UImage> CrossHair_Left;

	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	TObjectPtr<class UImage> CrossHair_Right;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UTexture2D> PortalL_Fill;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UTexture2D> PortalR_Fill;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UTexture2D> PortalL_Empty;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UTexture2D> PortalR_Empty;

public:
	void PortalUI_Empty(bool IsLeft);
	void PortalUI_Fill(bool IsLeft);
};
