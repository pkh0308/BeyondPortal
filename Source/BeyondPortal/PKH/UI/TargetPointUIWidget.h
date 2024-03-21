// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TargetPointUIWidget.generated.h"

/**
 * 
 */
UCLASS()
class BEYONDPORTAL_API UTargetPointUIWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UPROPERTY()
	TObjectPtr<class APlayerController> TargetController;

	UPROPERTY()
	TObjectPtr<class UWidgetComponent> TargetWidget;

	FVector2D ViewportSize;
	FVector2D CenterPoint;

	FVector2D ScreenLocation;

	bool IsActiveNow=false;

	void SetPointLocation();

public:
	FORCEINLINE void SetTargetWidget(class UWidgetComponent* NewWidget) { TargetWidget = NewWidget; }

	void Activate(bool IsActive);
};
