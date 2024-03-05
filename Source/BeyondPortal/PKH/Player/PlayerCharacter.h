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
	float PortalSpawnOffset=1.0f;

	FVector PortalExtent;

public:
	void SpawnPortal(const bool IsLeft, const FVector& Location, const FVector& Normal) const;

	void ResetAllPortals();

	FORCEINLINE FVector GetPortalExtent() const { return PortalExtent; }

	FORCEINLINE bool IsLeftPortal(AActor* Target) const { return Target->GetUniqueID() == PortalLeft->GetUniqueID(); }
	FORCEINLINE bool IsRightPortal(AActor* Target) const { return Target->GetUniqueID() == PortalLeft->GetUniqueID(); }

// Portal gun light
protected:
	UPROPERTY(EditDefaultsOnly)
	float MaxIntensity=50;

	float CurIntensity=0;

public:
	void PortalGunLightOn(FLinearColor NewColor);
	void PortalGunLightOff();

// Grab
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UParticleSystem> VFX_GrabEffect;

	class ICanGrab* GrabObject;

	float GrabDistance=200.0f;

public:
	void GrabObj(class ICanGrab* NewObject);
	void DropObj();
	FORCEINLINE ICanGrab* GetGrabObject() const { return GrabObject; }

	FVector GetGrabPoint() const;

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

// Network
protected:
	UFUNCTION(Server, Unreliable)
	void RPC_SpawnPortal(const bool IsLeft, const FVector& Location, const FVector& Normal, class APortal* LP, class APortal* RP) const;
};
