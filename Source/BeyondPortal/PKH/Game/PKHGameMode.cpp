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

	static ConstructorHelpers::FClassFinder<APlayerCharacter> P2CharacterClassRef(TEXT("/Game/PKH/Blueprint/BP_PlayerCharacter_P2.BP_PlayerCharacter_P2_C"));
	if ( P2CharacterClassRef.Class )
	{
		P2CharacterClass = P2CharacterClassRef.Class;
	}
}

void APKHGameMode::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(TimeHandle, FTimerDelegate::CreateLambda([this]()
	{
		Seconds++;
	}), 1.0f, false);
}

void APKHGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if( LogInCount == 1)
	{
		if(ACharacter* Character = NewPlayer->GetCharacter() )
		{
			NewPlayer->UnPossess();
			Character->Destroy();
		}
		
		APlayerCharacter* Player=GetWorld()->SpawnActor<APlayerCharacter>(P2CharacterClass, FVector(220, -548, 92), FRotator::ZeroRotator);
		NewPlayer->Possess(Player);
	}
	
	LogInCount++;
}

void APKHGameMode::AddCount(int32 Key, int32 Value)
{
	if( CountMap.Contains(Key) )
	{
		CountMap[Key] += Value;
	}
	else
	{
		CountMap.Add(Key, Value);
	}
}

int32 APKHGameMode::GetCount(int32 Key)
{
	if( CountMap.Contains(Key))
	{
		return CountMap[Key];
	}

	return -1;
}
