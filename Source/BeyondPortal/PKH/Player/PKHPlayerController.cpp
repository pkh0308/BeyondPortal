// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Player/PKHPlayerController.h"

APKHPlayerController::APKHPlayerController()
{

}

void APKHPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetInputMode(GameMode);
}
