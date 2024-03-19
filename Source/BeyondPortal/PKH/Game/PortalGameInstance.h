// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "PKH/UI/LobbyUIWidget.h"
#include "PortalGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class BEYONDPORTAL_API UPortalGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:
	virtual void Init() override;

// Variables
protected:
	IOnlineSessionPtr SessionInterface;

	FUniqueNetIdPtr MyNetID;
	
	FString HostName = TEXT("PSP");

// Create
public:
	// Request to create session
	void CreateRoom(int32 MaxPlayerCount, FString RoomName);

	// Response for create session
	void OnCreateRoomComplete(FName SessionName, bool bWasSuccessful);

// Find
protected:
	TSharedPtr<FOnlineSessionSearch> RoomSearchSettings;

	FOnFindRoomCompleteDelegate OnMyFindRoomDelegate;

public:
	// Request to find sessions
	void FindOtherRooms(FOnFindRoomCompleteDelegate OnFindRoomComplete);

	// Response for find sessions
	void OnFindOtherRoomsComplete(bool bWasSuccessful);

// Join
protected:
	FOnlineSessionSearchResult TargetSearchResult;

public:
	// Request to join session
	void JoinRoom(FString RoomName, FOnlineSessionSearchResult* result);

	// Response for join session
	void OnJoinRoomComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

// Enter Chapter
public:
	UFUNCTION(BlueprintCallable)
	void TravelToChapter1();

	UFUNCTION(BlueprintCallable)
	void TravelToChapter2();

// Timer
protected:
	int32 PlayTime = 0;

public:
	void AddPlayTime(int32 InPlayTime);

	FORCEINLINE int32 GetPlayTime() const { return PlayTime; }

// Portal Count
protected:
	int32 PortalCount_P1 = 0;
	int32 PortalCount_P2=0;

public:
	void AddPortalCount(int32 InPortalCont);

	int32 GetPortalCount() const;
};
