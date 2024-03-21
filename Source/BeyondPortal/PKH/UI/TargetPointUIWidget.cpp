// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/UI/TargetPointUIWidget.h"

#include "Components/WidgetComponent.h"

void UTargetPointUIWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TargetController=GetWorld()->GetFirstPlayerController();

	ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	CenterPoint = FVector2D(ViewportSize.X / 2, ViewportSize.Y / 2);

	MinPoint.X = 50;
	MinPoint.Y = 50;
	MaxPoint.X = ViewportSize.X - 50;
	MaxPoint.Y = ViewportSize.Y - 50;
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

		UE_LOG(LogTemp, Warning, TEXT("%f, %f"), TargetPoint.X, TargetPoint.Y)
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
}

void UTargetPointUIWidget::SetPointLocation()
{
	// Test
	TargetController->ProjectWorldLocationToScreen(TargetWidget->GetComponentLocation(), ScreenPoint, true);

	// Screen in
	if ( IsInScreen() )
	{
		SetPositionInViewport(ScreenPoint);
	}
	// Screen off
	else
	{
		const float DeltaX = ScreenPoint.X - CenterPoint.X;
		const float DeltaY = ScreenPoint.Y - CenterPoint.Y;
		if ( ScreenPoint.Y < MinPoint.Y ) // Up
		{
			TargetPoint.Y = MinPoint.Y;
			TargetPoint.X = FMath::Clamp(CenterPoint.X + (DeltaX * (TargetPoint.Y - CenterPoint.X)) / DeltaY, MinPoint.X, MaxPoint.X);
		}
		else if ( ScreenPoint.X > ViewportSize.X - 50 ) // Right
		{
			TargetPoint.X = MaxPoint.X;
			TargetPoint.Y = FMath::Clamp(CenterPoint.Y + ((TargetPoint.X - CenterPoint.X) * DeltaY) / DeltaX, MinPoint.Y, MaxPoint.Y);
		}
		else if ( ScreenPoint.Y > ViewportSize.Y - 50 ) // Down
		{
			TargetPoint.Y = MaxPoint.Y;
			TargetPoint.X = FMath::Clamp(CenterPoint.X + (DeltaX * (TargetPoint.Y - CenterPoint.Y)) / DeltaY, MinPoint.X, MaxPoint.X);
		}
		else // Left
		{
			TargetPoint.X = MinPoint.X;
			TargetPoint.Y =FMath::Clamp(CenterPoint.Y + ((TargetPoint.X - CenterPoint.X) * DeltaY) / DeltaX, MinPoint.Y, MaxPoint.Y);
		}
		SetPositionInViewport(TargetPoint);
	}
}

bool UTargetPointUIWidget::IsInScreen()
{
	if( ScreenPoint.X < MinPoint.X || ScreenPoint.X > MaxPoint.X )
	{
		return false;
	}
	if( ScreenPoint.Y < MinPoint.Y || ScreenPoint.Y > MaxPoint.Y )
	{
		return false;
	}

	return true;
}
