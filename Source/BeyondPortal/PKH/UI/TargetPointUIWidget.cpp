// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/UI/TargetPointUIWidget.h"

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

	if(IsActiveNow )
	{
		SetPointLocation();
	}
}

void UTargetPointUIWidget::Activate(bool IsActive)
{
	IsActiveNow=IsActive;
	if( IsActive )
	{
		SetVisibility(ESlateVisibility::Visible);
		SetPointLocation();
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
}

void UTargetPointUIWidget::SetPointLocation()
{
	// Test
	const bool IsInScreen=TargetController->ProjectWorldLocationToScreen(TargetWidget->GetComponentLocation(), ScreenLocation, true);

	// Screen in
	if ( IsInScreen )
	{
		SetPositionInViewport(ScreenLocation); //UE_LOG(LogTemp, Log, TEXT("%f, %f"), ScreenLocation.X, ScreenLocation.Y);
	}
	// Screen off
	else
	{
		FVector2D TargetPosition;
		if ( ScreenLocation.Y < 30 )
		{
			TargetPosition.Y=30;
			TargetPosition.X=(ScreenLocation.X * TargetPosition.Y) / ScreenLocation.Y;
		}
		else if ( ScreenLocation.X > ViewportSize.X - 30 )
		{
			TargetPosition.X=ViewportSize.X - 30;
			TargetPosition.Y=(TargetPosition.X * ScreenLocation.Y) / ScreenLocation.X;
		}
		else if ( ScreenLocation.Y > ViewportSize.Y - 30 )
		{
			TargetPosition.Y=ViewportSize.Y - 30;
			TargetPosition.X=(ScreenLocation.X * TargetPosition.Y) / ScreenLocation.Y;
		}
		else if ( ScreenLocation.X < 30 )
		{
			TargetPosition.X=30;
			TargetPosition.Y=(TargetPosition.X * ScreenLocation.Y) / ScreenLocation.X;
		}
		SetPositionInViewport(TargetPosition);
	}
}