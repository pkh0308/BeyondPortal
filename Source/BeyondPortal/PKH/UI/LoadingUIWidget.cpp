// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingUIWidget.h"

void ULoadingUIWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayAnimation(TextAnim, 0, 0);
}
