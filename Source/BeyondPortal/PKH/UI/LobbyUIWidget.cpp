// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/UI/LobbyUIWidget.h"

#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "PKH/Game/PortalGameInstance.h"

void ULobbyUIWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GameInst=GetWorld()->GetGameInstance<UPortalGameInstance>();
	check(GameInst);

	// Hide PopUp
	PopUp_WaitPlayer->SetVisibility(ESlateVisibility::Hidden);

	//Bind
	Btn_StartGame->OnClicked.AddDynamic(this, &ULobbyUIWidget::OnClicked_StartGame);

	OnFindRoomComplete.BindUObject(this, &ULobbyUIWidget::DoesRoomExist);
}

void ULobbyUIWidget::OnClicked_StartGame()
{
	GameInst->FindOtherRooms(OnFindRoomComplete);
}

void ULobbyUIWidget::DoesRoomExist(bool bExist)
{
	// 이미 방이 있다면 Join
	if( bExist )
	{
		GameInst->JoinRoom(TEXT("BeyondPortal"));
	}
	// 방이 없다면 새로 Create & 팝업 띄우기
	else
	{
		GameInst->CreateRoom(2, TEXT("BeyondPortal"));
		PopUp_WaitPlayer->SetVisibility(ESlateVisibility::Visible);
	}
}
