// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/UI/CrosshairUIWidget.h"

#include "Components/Image.h"

UCrosshairUIWidget::UCrosshairUIWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UCrosshairUIWidget::NativeConstruct()
{
	Super::NativeConstruct();


}

void UCrosshairUIWidget::PortalUI_Empty(bool IsLeft)
{
	if(IsLeft)
	{
		CrossHair_Left->SetBrushFromTexture(PortalL_Empty);
	}
	else
	{
		CrossHair_Right->SetBrushFromTexture(PortalR_Empty);
	}
}

void UCrosshairUIWidget::PortalUI_Fill(bool IsLeft)
{
	if ( IsLeft )
	{
		CrossHair_Left->SetBrushFromTexture(PortalL_Fill);
	}
	else
	{
		CrossHair_Right->SetBrushFromTexture(PortalR_Fill);
	}
}
