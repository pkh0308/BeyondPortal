// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Anim/PlayerAnimInstance.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "PKH/Player/PlayerCharacter.h"

UPlayerAnimInstance::UPlayerAnimInstance()
{
	WalkThreshold=10.0f;

	// Emotions
	static ConstructorHelpers::FObjectFinder<UAnimMontage> EmotionGreetingRef(TEXT("/Script/Engine.AnimMontage'/Game/PKH/Animation/AM_Greeting.AM_Greeting'"));
	if( EmotionGreetingRef.Object )
	{
		Emotions.Add(EmotionGreetingRef.Object);
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage> EmotionClapRef(TEXT("/Script/Engine.AnimMontage'/Game/PKH/Animation/AM_Clap.AM_Clap'"));
	if ( EmotionClapRef.Object )
	{
		Emotions.Add(EmotionClapRef.Object);
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage> EmotionThumbDownRef(TEXT("/Script/Engine.AnimMontage'/Game/PKH/Animation/AM_ThumbDown.AM_ThumbDown'"));
	if ( EmotionThumbDownRef.Object )
	{
		Emotions.Add(EmotionThumbDownRef.Object);
	}
}

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner=Cast<APlayerCharacter>(GetOwningActor());
	if(Owner)
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

void UPlayerAnimInstance::PlayMontage_Dead()
{
	if ( Montage_Dead )
	{
		Montage_Play(Montage_Dead);
	}
}

void UPlayerAnimInstance::PlayMontage_Emotion(float Degree)
{
	if(Emotions.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("There is no emotion montage"));
		return;
	}

	int idx = Degree / (360 / Emotions.Num()); 
	if(Emotions[idx])
	{
		Montage_Play(Emotions[idx]);
	}
}

void UPlayerAnimInstance::AnimNotify_EmotionEnd()
{
	Owner->EndEmotion();
}
