// Copyright Epic Games, Inc. All Rights Reserved.

#include "BeyondPortalGameMode.h"
#include "BeyondPortalCharacter.h"
#include "UObject/ConstructorHelpers.h"

ABeyondPortalGameMode::ABeyondPortalGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
