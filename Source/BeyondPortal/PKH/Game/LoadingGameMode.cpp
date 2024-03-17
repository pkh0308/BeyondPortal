// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Game/LoadingGameMode.h"

#include "PortalGameInstance.h"

ALoadingGameMode::ALoadingGameMode()
{

}

void ALoadingGameMode::BeginPlay()
{
	Super::BeginPlay();


}

void ALoadingGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if(GetNumPlayers() == 2)
	{
		if(HasAuthority())
		{
			UPortalGameInstance* _GameInst = CastChecked<UPortalGameInstance>(GetGameInstance());
			_GameInst->TravelToChapter1();
		}
	}
}
