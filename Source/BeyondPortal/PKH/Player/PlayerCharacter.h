// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PKH/Portal/Portal.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class BEYONDPORTAL_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

// Component
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UStaticMeshComponent> GunComp;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class USkeletalMeshComponent> MeshComp;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UCharacterMovementComponent> MoveComp;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UPlayerInputComponent> InputComp;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UCameraComponent> CameraComp;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UPointLightComponent> LightComp;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UParticleSystemComponent> GunParticleComp;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UPhysicsHandleComponent> PhysicsHandleComp;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UWidgetComponent> TargetUIComp;

public:
	FORCEINLINE UCameraComponent* GetCameraComp()const  { return CameraComp; }

// Portal
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class APortal> PortalLClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class APortal> PortalRClass;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class APortal> PortalLeft;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class APortal> PortalRight;

	UPROPERTY(EditDefaultsOnly)
	float PortalSpawnOffset=0.5f;

	FVector PortalExtent;

protected:
	void Spawn(const bool IsLeft, const FVector& Location, const FVector& Normal);

public:
	bool IsOverlapPortal(bool IsLeft, FVector TargetCenter);

	void SpawnPortal(const bool IsLeft, const FVector& Location, const FVector& Normal);

	void ResetAllPortals();

	void ChangeVelocity(const FVector& NewDirection);

	FORCEINLINE FVector GetPortalExtent() const { return PortalExtent; }

	FORCEINLINE bool IsLeftPortal(AActor* Target) const { return Target->GetUniqueID() == PortalLeft->GetUniqueID(); }
	FORCEINLINE bool IsRightPortal(AActor* Target) const { return Target->GetUniqueID() == PortalLeft->GetUniqueID(); }

// Portal - Network
protected:
	UFUNCTION(Server, Unreliable)
	void RPC_Server_InitPortal();

// Portal
protected:
	int32 PortalCount=0;

public:
	FORCEINLINE void AddPortalCount() { ++PortalCount; UE_LOG(LogTemp, Warning, TEXT("PortalCount: %d"), PortalCount) }
	FORCEINLINE int32 GetPortalCount() const { return PortalCount; }

// Overlap
protected:
	UFUNCTION()
	void OnPlayerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

// Portal gun light
protected:
	UPROPERTY(EditDefaultsOnly)
	float MaxIntensity=50;

	float CurIntensity=0;

	UPROPERTY(EditDefaultsOnly)
	FLinearColor LColor;

	UPROPERTY(EditDefaultsOnly)
	FLinearColor RColor;

public:
	void PortalGunLightOn(bool IsLeft);
	void PortalGunLightOff();

// Camera
public:
	FVector GetCameraLocation() const;

// Portal
public:
	void PortalOut(const FVector& NewLocation, const FRotator& NewRotation, const FVector& NewDirection);

	UFUNCTION(NetMulticast, Reliable)
	void RPC_Server_PortalOut(const FVector& NewLocation, const FRotator& NewRotation, const FVector& NewDirection);

	UFUNCTION(NetMulticast, Reliable)
	void RPC_Multi_PortalOut(const FVector& NewLocation, const FRotator& NewRotation, const FVector& NewDirection);

	UFUNCTION(Server, Reliable)
	void RPC_Server_SpawnPortal(bool IsLeft, const FVector& NewLocation, FRotator NewRotation, const FVector& Normal) const;

	UFUNCTION(NetMulticast, Reliable)
	void RPC_Multi_SpawnPortal(bool IsLeft, const FVector& NewLocation, FRotator NewRotation, const FVector& Normal) const;

	UFUNCTION(Server, Reliable)
	void RPC_Server_LinkPortal() const;

	UFUNCTION(NetMulticast, Reliable)
	void RPC_Multi_LinkPortal() const;

// Particle
	void SpawnFail(class UParticleSystem* TargetVFX, const FVector& NewLocation, const FRotator& NewRotation) const;

	UFUNCTION(Server, Reliable)
	void RPC_Server_SpawnFail(class UParticleSystem* TargetVFX, const FVector& NewLocation, const FRotator& NewRotation) const;

	UFUNCTION(NetMulticast, Reliable)
	void RPC_Multi_SpawnFail(class UParticleSystem* TargetVFX, const FVector& NewLocation, const FRotator& NewRotation) const;

// Look
public:
	void Look(float PItchInput, float YawInput);

	UFUNCTION(Server, Reliable)
	void RPC_Server_Look(float PItchInput, float YawInput);

	void SetClientCameraRotation();

	UFUNCTION(Server, Reliable)
	void RPC_Server_SetCameraRotation(FRotator NewRotation);

// Grab
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UParticleSystem> VFX_GrabEffect;

	class ICanGrab* GrabObject;

	float GrabDistance=190.0f;

	FVector GrabPointOffset = FVector(0, 0, 15);

	void TickGrab();

public:
	void GrabObj(class ICanGrab* NewObject, UPrimitiveComponent* TargetComp);
	void DropObj();
	FORCEINLINE ICanGrab* GetGrabObject() const { return GrabObject; }

	FVector GetGrabPoint() const;

// InterAction RPC
public:
	UFUNCTION(Server, Reliable)
	void RPC_Server_Interaction(float InteractionDistance);

	UFUNCTION(NetMulticast, Reliable)
	void RPC_Multi_GrabObj(UPrimitiveComponent* TargetComp);

	UFUNCTION(NetMulticast, Reliable)
	void RPC_Multi_DropObj();

// Emotion
protected:
	UPROPERTY(EditDefaultsOnly)
	FVector CameraLocationInEmotion=FVector(-300, 0, 80);

	UPROPERTY(EditDefaultsOnly)
	FVector CameraLocationInNormal=FVector(17, 7, 28);

	UFUNCTION(Server, Reliable)
	void RPC_Server_Emotion(float Degree);

	UFUNCTION(NetMulticast, Reliable)
	void RPC_Multi_Emotion(float Degree);

	bool IsShowingEmotion = false;

public:
	void SetEmotionUI(bool ActiveSelf);

	void BeginEmotion();
	void EndEmotion();

	FORCEINLINE bool GetIsShowingEmotion() const { return IsShowingEmotion; }

// TargetUI
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UTargetUIWidget> TargetUIClass;

public:
	void SetTargetUI();

	UFUNCTION(Server, Reliable)
	void RPC_Server_SetTargetUI();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_Multi_SetTargetUI(const FVector& HitLocation, const FRotator& HitRotation);

	UFUNCTION(NetMulticast, Reliable)
	void RPC_Multi_TargetUIOff();

// Die & Respawn
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AGunActor> GunActorClass;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class AGunActor> GunActor;

	FVector CameraDefaultLocation=FVector(17, 7, 28);
	FVector CameraOnDieLocation=FVector(-400, 0, 150);

	FVector RespawnLocation;

	bool IsDead=false;

	void Respawn();

public:
	void OnDie();

	FORCEINLINE bool IsPlayerDead() const { return IsDead; }

// Animation
protected:
	bool IsCrouching=false;

public:
	FORCEINLINE bool GetIsCrouching() const { return IsCrouching; }

	void PlayerCrouch();

// UI
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UCrosshairUIWidget> CrosshairUIClass;;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UCrosshairUIWidget> CrosshairUI;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UGameClearUIWidget> GameClearUIClass;;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UGameClearUIWidget> GameClearUI;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UEmotionUIWidget> EmotionUIClass;;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UEmotionUIWidget> EmotionUI;

public:
	void CrosshairFill(bool IsLeft);

	void GameClear();

	UFUNCTION(NetMulticast, Reliable)
	void RPC_Multi_GameClear(int32 PlayTime);

// Network
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

// RPC
protected:
	UFUNCTION(Server, Unreliable)
	void RPC_SetPlayerLocation(class ACharacter* ClientPlayer);

// Sound
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class USoundBase> SFX_PortalLeft;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class USoundBase> SFX_PortalRight;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class USoundBase> SFX_PortalOutWeak;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class USoundBase> SFX_PortalOutStrong;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class USoundBase> SFX_PortalFail;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class USoundBase> SFX_Grab;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class USoundBase> SFX_GrabLoop;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class USoundBase> SFX_Drop;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UAudioComponent> GunSoundComp;
};
