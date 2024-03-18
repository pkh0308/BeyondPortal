// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/UI/GameClearUIWidget.h"

#include "Components/TextBlock.h"

void UGameClearUIWidget::SetClearUI(int32 PlayTime, int32 PortalCount)
{
	// PlayTime
	const int32 Minutes=PlayTime / 60; 
	const int32 Seconds=PlayTime % 60;
	const FString MM = Minutes < 10 ? FString::Printf(TEXT("0%d"), Minutes) : FString::Printf(TEXT("%d"), Minutes);
	const FString SS = Seconds < 10 ? FString::Printf(TEXT("0%d"), Seconds) : FString::Printf(TEXT("%d"), Seconds);
	Txt_PlayTime->SetText(FText::FromString(FString::Printf(TEXT("%s : %s"), *MM, *SS)));

	// PortalCount
	Txt_PortalCount->SetText(FText::FromString(FString::Printf(TEXT("%d개"), PortalCount)));
}
