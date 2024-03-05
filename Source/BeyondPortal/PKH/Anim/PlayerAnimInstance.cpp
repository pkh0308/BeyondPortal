// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Anim/PlayerAnimInstance.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "PKH/Player/PlayerCharacter.h"

UPlayerAnimInstance::UPlayerAnimInstance()
{
	WalkThreshold=10.0f;
}

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner=Cast<APlayerCharacter>(GetOwningActor());
	if(Owner )
	{
		PlayerMoveComp=Owner->GetCharacterMovement();
	}
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if(nullptr == PlayerMoveComp )
	{
		return;
	}

	bIsIdle=PlayerMoveComp->Velocity.Size2D() < WalkThreshold;
	bIsWalking =PlayerMoveComp->Velocity.Size2D() > WalkThreshold;
	bIsFalling=PlayerMoveComp->IsFalling();
	bIsCrouching=Owner->GetIsCrouching();

	ControllerPitch=Owner->GetControlRotation().GetNormalized().Pitch * -1;
	if( ControllerPitch < 0 )
	{
		ControllerPitch *= 0.5f;
	}
	ControllerPitch=FMath::Clamp(ControllerPitch, -30, 60);
}

void UPlayerAnimInstance::PlayMontage_Fire()
{
	

	if( Montage_Fire )
	{
		Montage_Play(Montage_Fire);
	}
}

