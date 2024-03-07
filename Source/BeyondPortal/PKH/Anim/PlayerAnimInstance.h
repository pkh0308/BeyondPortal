// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class BEYONDPORTAL_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPlayerAnimInstance();

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

// Owner
protected:
	UPROPERTY()
	TObjectPtr<class APlayerCharacter> Owner;

	UPROPERTY()
	TObjectPtr<class UCharacterMovementComponent> PlayerMoveComp;

// State
protected:
	float WalkThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = PlayerState)
	uint8 bIsIdle : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category=PlayerState)
	uint8 bIsWalking : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category=PlayerState)
	uint8 bIsFalling : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category=PlayerState)
	uint8 bIsCrouching : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category=PlayerState)
	float ControllerPitch;

// Montage
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UAnimMontage> Montage_Fire;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UAnimMontage> Montage_Dead;

public:
	void PlayMontage_Fire();
	void PlayMontage_Dead();
};
