// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Game/LobbyGameMode.h"

#include "Blueprint/UserWidget.h"
#include "PKH/UI/LobbyUIWidget.h"

ALobbyGameMode::ALobbyGameMode()
{
	static ConstructorHelpers::FClassFinder<ULobbyUIWidget> LobbyUIClassRef(TEXT("/Game/PKH/UI/WBP_Lobby.WBP_Lobby_C"));
	if( LobbyUIClassRef.Class)
	{
		LobbyUIClass=LobbyUIClassRef.Class;
	}
}

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	LobbyUI=Cast<ULobbyUIWidget>(CreateWidget(GetWorld(), LobbyUIClass));
	if( LobbyUI )
	{
		LobbyUI->AddToViewport();
	}

	auto _Controller=GetWorld()->GetFirstPlayerController();
	if(_Controller )
	{
		_Controller->SetShowMouseCursor(true);
		_Controller->SetInputMode(FInputModeUIOnly());
	}
}
