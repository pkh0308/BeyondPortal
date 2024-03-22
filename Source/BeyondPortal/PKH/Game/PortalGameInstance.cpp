// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Game/PortalGameInstance.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "PKHGameMode.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Online/OnlineSessionNames.h"
#include "Interfaces/OnlineSessionInterface.h"

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
	CurRoomName=RoomName;
	
	ULocalPlayer* _Player=GetWorld()->GetFirstLocalPlayerFromController();
	SessionInterface->CreateSession(*_Player->GetPreferredUniqueNetId(), FName(*RoomName), Settings);
	UE_LOG(LogTemp, Warning, TEXT("ID: %s"), *_Player->GetPreferredUniqueNetId()->ToString());
}

void UPortalGameInstance::OnCreateRoomComplete(FName SessionName, bool bWasSuccessful)
{
	if(false == bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Create Room Failed: %s"), *SessionName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Create Room Success: %s"), *SessionName.ToString());

		// 레벨 입장
		UWorld* _World=GetWorld();
		ensure(_World);
		if ( GetWorld()->GetFirstPlayerController()->HasAuthority() )
		{
			GetWorld()->ServerTravel("/Game/PKH/Maps/LoadingMap?listen");
		}
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
	SessionInterface->FindSessions(0, RoomSearchSettings.ToSharedRef());
}

void UPortalGameInstance::OnFindOtherRoomsComplete(bool bWasSuccessful)
{
	if ( !bWasSuccessful )
	{
		OnMyFindRoomDelegate.ExecuteIfBound(false, nullptr);
		return;
	}

	if( RoomSearchSettings->SearchResults.Num() > 0 )
	{
		for( auto Result : RoomSearchSettings->SearchResults )
		{
			if ( false == Result.IsValid() )
			{
				continue;
			}

			FString RoomName;
			Result.Session.SessionSettings.Get(TEXT("ROOM_NAME"), RoomName);
			UE_LOG(LogTemp, Warning, TEXT("Room Name: %s"), *RoomName);

			if ( RoomName.Equals("BeyondPortal") )
			{
				UE_LOG(LogTemp, Warning, TEXT("Find Room Success"));
				TargetSearchResult=Result;
				OnMyFindRoomDelegate.ExecuteIfBound(true, &Result);
				return;
			}
		}
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Find Room Failed"));
	OnMyFindRoomDelegate.ExecuteIfBound(false, nullptr);
}

void UPortalGameInstance::JoinRoom(FString RoomName, FOnlineSessionSearchResult* result)
{
	ULocalPlayer* _Player=GetWorld()->GetFirstLocalPlayerFromController();
	SessionInterface->JoinSession(*_Player->GetPreferredUniqueNetId(), FName(*RoomName), *result);
	UE_LOG(LogTemp, Warning, TEXT("ID: %s"), *_Player->GetPreferredUniqueNetId()->ToString());
}

void UPortalGameInstance::OnJoinRoomComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if( Result == EOnJoinSessionCompleteResult::Success )
	{
		UE_LOG(LogTemp, Warning, TEXT("Join Success"));

		if(false == SessionInterface.IsValid() )
		{
			return;
		}

		APlayerController* PlayerController=GetFirstLocalPlayerController();
		if(false == IsValid(PlayerController) )
		{
			return;
		}

		FString TravelURL;
		if ( PlayerController && SessionInterface->GetResolvedConnectString(SessionName, TravelURL) )
		{
			UE_LOG(LogTemp, Warning, TEXT("Client Travel: %s"), *TravelURL);
			PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Join Failed"));
		UE_LOG(LogTemp, Warning, TEXT("Users: %d"), GetWorld()->GetAuthGameMode()->GetNumPlayers());
	}
}

void UPortalGameInstance::ExitRoom()
{
	RPC_Server_ExitRoom();
}

void UPortalGameInstance::RPC_Server_ExitRoom_Implementation()
{
	RPC_Multi_ExitRoom();
}

void UPortalGameInstance::RPC_Multi_ExitRoom_Implementation()
{
	SessionInterface->DestroySession(FName(*CurRoomName));
}

void UPortalGameInstance::OnExitRoomComplete(FName RoomName, bool bWasSuccessful)
{
	if ( bWasSuccessful )
	{
		CurRoomName="";
		GetWorld()->GetFirstPlayerController()->ClientTravel(LobbyURL, TRAVEL_Absolute);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Destroy Room Failed"));
	}
}

void UPortalGameInstance::TravelToChapter1()
{
	// 레벨 입장
	UWorld* _World=GetWorld();
	ensure(_World);
	if ( _World->GetFirstPlayerController()->HasAuthority() )
	{
		_World->ServerTravel("/Game/SEB/Maps/BeyondPortalMap?listen");
	}
}

void UPortalGameInstance::TravelToChapter2()
{
	// 플레이타임 누적
	APKHGameMode* GM=Cast<APKHGameMode>(GetWorld()->GetAuthGameMode());
	if( GM )
	{
		AddPlayTime(GM->GetPlayTime());
	}

	// 레벨 입장
	UWorld* _World=GetWorld();
	ensure(_World);
	if ( _World->GetFirstPlayerController()->HasAuthority() )
	{
		_World->ServerTravel("/Game/SEB/Maps/BeyondPortalMap_Chapter2?listen");
	}
}

void UPortalGameInstance::AddPlayTime(int32 InPlayTime)
{
	PlayTime += InPlayTime;
}

int32 UPortalGameInstance::GetPortalCount() const
{
	return -1;
}
