// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/UI/TargetPointUIWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/WidgetComponent.h"

void UTargetPointUIWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TargetController=GetWorld()->GetFirstPlayerController();

	ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	CenterPoint = FVector2D(ViewportSize.X / 2, ViewportSize.Y / 2);
}

void UTargetPointUIWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(false == IsInViewport())
	{
		return;
	}

	// Test
	UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(TargetController, TargetWidget->GetComponentLocation(), ScreenLocation, true);
	ScreenLocation.Y = ViewportSize.Y - ScreenLocation.Y;
	
	// Screen in
	if( IsInScreen() )
	{
		SetPositionInViewport(ScreenLocation);
	}
	// Screen off
	else
	{
		/*FVector2D TargetPosition;
		if( ScreenLocation.X > ScreenLocation.Y )
		{
			TargetPosition.X = ViewportSize.X;
		}
		else
		{
			TargetPosition.Y = ViewportSize.Y;
		}*/
	}
}

bool UTargetPointUIWidget::IsInScreen()
{
	if ( ScreenLocation.X < 0 || ScreenLocation.X > ViewportSize.X )
	{
		return false;
	}
	if ( ScreenLocation.Y < 0 || ScreenLocation.Y > ViewportSize.Y )
	{
		return false;
	}

	return true;
}
