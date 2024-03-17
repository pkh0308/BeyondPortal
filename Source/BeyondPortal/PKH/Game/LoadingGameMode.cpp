// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Game/LoadingGameMode.h"

#include "PortalGameInstance.h"

ALoadingGameMode::ALoadingGameMode()
{
	static ConstructorHelpers::FClassFinder<ULobbyUIWidget> LoadingUIClassRef(TEXT("/Game/PKH/UI/WBP_Loading.WBP_Loading_C"));
	if ( LoadingUIClassRef.Class )
	{
		LoadingUIClass=LoadingUIClassRef.Class;
	}
}

void ALoadingGameMode::BeginPlay()
{
	Super::BeginPlay();

	LoadingUI=Cast<ULobbyUIWidget>(CreateWidget(GetWorld(), LoadingUIClass));
	if ( LoadingUI )
	{
		LoadingUI->AddToViewport();
	}

	auto _Controller=GetWorld()->GetFirstPlayerController();
	if ( _Controller )
	{
		_Controller->SetShowMouseCursor(true);
		_Controller->SetInputMode(FInputModeUIOnly());
	}
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
