// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Player/PlayerInputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerCharacter.h"
#include "InputMappingContext.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PKH/Interface/Interactible.h"

UPlayerInputComponent::UPlayerInputComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Input
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMCRef(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/PKH/Input/IMC_Player.IMC_Player'"));
	if( IMCRef.Object )
	{
		IMC = IMCRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_MoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/PKH/Input/IA_Portal_Move.IA_Portal_Move'"));
	if ( IA_MoveRef.Object )
	{
		IA_Move = IA_MoveRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_LookRef(TEXT("/Script/EnhancedInput.InputAction'/Game/PKH/Input/IA_Portal_Look.IA_Portal_Look'"));
	if ( IA_LookRef.Object )
	{
		IA_Look=IA_LookRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_JumpRef(TEXT("/Script/EnhancedInput.InputAction'/Game/PKH/Input/IA_Portal_Jump.IA_Portal_Jump'"));
	if ( IA_JumpRef.Object )
	{
		IA_Jump=IA_JumpRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_CrouchRef(TEXT("/Script/EnhancedInput.InputAction'/Game/PKH/Input/IA_Portal_Crouch.IA_Portal_Crouch'"));
	if ( IA_CrouchRef.Object )
	{
		IA_Crouch=IA_CrouchRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_FireLeftRef(TEXT("/Script/EnhancedInput.InputAction'/Game/PKH/Input/IA_Portal_FireLeft.IA_Portal_FireLeft'"));
	if ( IA_FireLeftRef.Object )
	{
		IA_FireLeft=IA_FireLeftRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_FireRightRef(TEXT("/Script/EnhancedInput.InputAction'/Game/PKH/Input/IA_Portal_FireRight.IA_Portal_FireRight'"));
	if ( IA_FireRightRef.Object )
	{
		IA_FireRight=IA_FireRightRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_InteractionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/PKH/Input/IA_Portal_Grab.IA_Portal_Grab'"));
	if ( IA_InteractionRef.Object )
	{
		IA_Interaction=IA_InteractionRef.Object;
	}
}

void UPlayerInputComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner=Cast<APlayerCharacter>(GetOwner());
	if( nullptr == Owner )
	{
		return;
	}
	MoveComp=Owner->GetCharacterMovement();

	// Input Mapping
	APlayerController* MyController=Cast<APlayerController>(Owner->GetController());
	if( MyController )
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem=ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(MyController->GetLocalPlayer());
		if ( Subsystem )
		{
			Subsystem->ClearAllMappings();
			Subsystem->AddMappingContext(IMC, 0);
		}
	}
}

void UPlayerInputComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}

void UPlayerInputComponent::InitializeComponent()
{
	Super::InitializeComponent();

	
}

void UPlayerInputComponent::SetupInput(UEnhancedInputComponent* PlayerInputComponent)
{
	UEnhancedInputComponent* InputComp=CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	// Move
	InputComp->BindAction(IA_Move, ETriggerEvent::Triggered, this, &UPlayerInputComponent::OnIAMove);
	// Look
	InputComp->BindAction(IA_Look, ETriggerEvent::Triggered, this, &UPlayerInputComponent::OnIALook);
	// Jump
	InputComp->BindAction(IA_Jump, ETriggerEvent::Started, this, &UPlayerInputComponent::OnIAJump);
	// Crouch
	InputComp->BindAction(IA_Crouch, ETriggerEvent::Started, this, &UPlayerInputComponent::OnIACrouch);
	InputComp->BindAction(IA_Crouch, ETriggerEvent::Completed, this, &UPlayerInputComponent::OnIACrouch);

	// Fire
	InputComp->BindAction(IA_FireLeft, ETriggerEvent::Started, this, &UPlayerInputComponent::OnIAFireLeft);
	InputComp->BindAction(IA_FireRight, ETriggerEvent::Started, this, &UPlayerInputComponent::OnIAFireRight);
	// Grab
	InputComp->BindAction(IA_Interaction, ETriggerEvent::Started, this, &UPlayerInputComponent::OnIAInteraction);
}

void UPlayerInputComponent::OnIAMove(const FInputActionValue& Value)
{
	const FVector2D InputVec=Value.Get<FVector2D>();

	const FRotator Rotator=Owner->Controller->GetControlRotation();
	const FRotator YawRotator=FRotator(0, Rotator.Yaw, 0);

	const FVector ForwardVec=FRotationMatrix(YawRotator).GetUnitAxis(EAxis::X);
	const FVector RightVec=FRotationMatrix(YawRotator).GetUnitAxis(EAxis::Y);

	Owner->AddMovementInput(ForwardVec, InputVec.X);
	Owner->AddMovementInput(RightVec, InputVec.Y);
}

void UPlayerInputComponent::OnIALook(const FInputActionValue& Value)
{
	const FVector2D InputVec=Value.Get<FVector2D>();

	Owner->AddControllerPitchInput(InputVec.X * MouseSensitivity);
	Owner->AddControllerYawInput(InputVec.Y * MouseSensitivity);
}

void UPlayerInputComponent::OnIAJump(const FInputActionValue& Value)
{
	Owner->Jump();
}

void UPlayerInputComponent::OnIACrouch(const FInputActionValue& Value)
{
	Owner->PlayerCrouch();
}

void UPlayerInputComponent::OnIAFireLeft(const FInputActionValue& Value)
{
	FHitResult HitResult;
	if( TrySpawnPortal(HitResult) )
	{
		Owner->SpawnPortal(true, HitResult.ImpactPoint, HitResult.ImpactNormal);
	}
}

void UPlayerInputComponent::OnIAFireRight(const FInputActionValue& Value)
{
	FHitResult HitResult;
	if ( TrySpawnPortal(HitResult) )
	{
		Owner->SpawnPortal(false, HitResult.ImpactPoint, HitResult.ImpactNormal);
	}
}

void UPlayerInputComponent::OnIAInteraction(const FInputActionValue& Value)
{
	FHitResult HitResult;
	APlayerCameraManager* CameraManager=UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	const FVector StartVec=CameraManager->GetCameraLocation();
	const FVector EndVec=StartVec + CameraManager->GetActorForwardVector() * InteractionDistance;

	bool IsHit=GetWorld()->LineTraceSingleByChannel(HitResult, StartVec, EndVec, ECC_Camera);
	if(false == IsHit)
	{
		return;
	}

	// Interaction
	IInteractible* InteractibleActor = Cast<IInteractible>(HitResult.GetActor());
	if(nullptr == InteractibleActor )
	{
		return;
	}

	InteractibleActor->DoInteraction();
}

bool UPlayerInputComponent::TrySpawnPortal(FHitResult& InHitResult) const
{
	APlayerCameraManager* CameraManager=UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	const FVector StartVec=CameraManager->GetCameraLocation();
	const FVector EndVec=StartVec + CameraManager->GetActorForwardVector() * 10000;

	FCollisionQueryParams Param;
	Param.AddIgnoredActor(Owner);
	bool IsHit=GetWorld()->LineTraceSingleByChannel(InHitResult, StartVec, EndVec, ECC_GameTraceChannel18, Param);
	if ( false == IsHit )
	{
		
		return false;
	}

	return true;
}
