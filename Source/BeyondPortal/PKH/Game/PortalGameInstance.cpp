// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Game/PortalGameInstance.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Online/OnlineSessionNames.h"

void UPortalGameInstance::Init()
{
	Super::Init();

	if(IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get() )
	{
		SessionInterface=Subsystem->GetSessionInterface();
		SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPortalGameInstance::OnCreateRoomComplete);
		SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPortalGameInstance::OnFindOtherRoomsComplete);
		SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPortalGameInstance::OnJoinRoomComplete);
	}
}

void UPortalGameInstance::CreateRoom(int32 MaxPlayerCount, FString RoomName)
{
	FOnlineSessionSettings Settings;

	// 1. 데디케이트 서버인가??
	Settings.bIsDedicated=false;
	// 2. 랜선인가?
	auto subSys=IOnlineSubsystem::Get();
	Settings.bIsLANMatch=subSys->GetSubsystemName().IsEqual("NULL");
	// 3. 공개로 입장할 수 있는가? 아니면 친구초대로만???
	Settings.bShouldAdvertise=true;
	// 4. 온라인 상태(presence)를 공개적으로 사용할것인가? -> ping정보
	Settings.bUsesPresence=true;
	// 5. 중간입장이 가능한가?
	Settings.bAllowJoinInProgress=true;
	Settings.bAllowJoinViaPresence=true;
	// 6. 최대 입장 가능한 수 설정
	Settings.NumPublicConnections=MaxPlayerCount;
	// 7. 커스텀 정보 설정
	Settings.Set(TEXT("HOST_NAME"), HostName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	Settings.Set(TEXT("ROOM_NAME"), RoomName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	UE_LOG(LogTemp, Warning, TEXT("ID: %s"), *MyNetID->ToString());
	SessionInterface->CreateSession(12184357, FName(*RoomName), Settings);
}

void UPortalGameInstance::OnCreateRoomComplete(FName SessionName, bool bWasSuccessful)
{
	if(false == bWasSuccessful)
	{
		return;
	}

	for ( auto r : RoomSearchSettings->SearchResults )
	{
		if ( false == r.IsValid() )
			continue;

		FString RoomName;
		r.Session.SessionSettings.Get(TEXT("ROOM_NAME"), RoomName);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *RoomName);
	}
}

void UPortalGameInstance::FindOtherRooms(FOnFindRoomCompleteDelegate OnFindRoomComplete)
{
	OnMyFindRoomDelegate=OnFindRoomComplete;

	// 1. FOnlineSessionSearch객체를 생성
	RoomSearchSettings=MakeShareable(new FOnlineSessionSearch());
	// 2. 세션 검색 조건 설정
	RoomSearchSettings->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	// 2-1. 최대 검색 갯수를 정하고싶다.
	RoomSearchSettings->MaxSearchResults=10;
	// 2-2. 랜선인지 아닌지를 정하고싶다.
	auto SubSystem=IOnlineSubsystem::Get();
	RoomSearchSettings->bIsLanQuery=SubSystem->GetSubsystemName().IsEqual("NULL");
	// 3. 검색을 하고싶다.
	MyNetID=GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();
	UE_LOG(LogTemp, Warning, TEXT("ID: %s"), *MyNetID->ToString());
	SessionInterface->FindSessions(12184357, RoomSearchSettings.ToSharedRef());
}

void UPortalGameInstance::OnFindOtherRoomsComplete(bool bWasSuccessful)
{
	if( RoomSearchSettings->SearchResults.Num() > 0 )
	{
		UE_LOG(LogTemp, Warning, TEXT("Find Room Success"));
		OnMyFindRoomDelegate.ExecuteIfBound(true);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Find Room Failed"));
		OnMyFindRoomDelegate.ExecuteIfBound(false);
	}
}

void UPortalGameInstance::JoinRoom(FString RoomName)
{
	FOnlineSessionSearchResult SearchResult;
	UE_LOG(LogTemp, Warning, TEXT("ID: %s"), *MyNetID->ToString());
	SessionInterface->JoinSession(12184357, FName(*RoomName), SearchResult);
}

void UPortalGameInstance::OnJoinRoomComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if( Result == EOnJoinSessionCompleteResult::Success )
	{
		UE_LOG(LogTemp, Warning, TEXT("Join Success"));

		if(GetWorld()->GetAuthGameMode()->GetNumPlayers() == 2)
		{
			UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("BeyondPortalMap")));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Join Failed"));
		UE_LOG(LogTemp, Warning, TEXT("Users: %d"), GetWorld()->GetAuthGameMode()->GetNumPlayers());
	}
}
