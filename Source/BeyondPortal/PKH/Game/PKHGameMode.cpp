// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Game/PKHGameMode.h"

#include "PKH/Player/PKHPlayerController.h"
#include "PKH/Player/PlayerCharacter.h"

APKHGameMode::APKHGameMode()
{
	static ConstructorHelpers::FClassFinder<APlayerCharacter> PlayerRef(TEXT("/Game/PKH/Blueprint/BP_PlayerCharacter.BP_PlayerCharacter_C"));
	if(PlayerRef.Class )
	{
		DefaultPawnClass=PlayerRef.Class;
	}

	static ConstructorHelpers::FClassFinder<APKHPlayerController> ControllerRef(TEXT("/Game/PKH/Blueprint/BP_PKHPlayerController.BP_PKHPlayerController_C"));
	if( ControllerRef.Class )
	{
		PlayerControllerClass=ControllerRef.Class;
	}
}
