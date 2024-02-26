// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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

public:
	void SpawnPortal(const bool IsLeft, const FVector& Location, const FVector& Normal) const;

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
};
