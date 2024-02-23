// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Player/PlayerInputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerCharacter.h"
#include "InputMappingContext.h"

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
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_GrabRef(TEXT("/Script/EnhancedInput.InputAction'/Game/PKH/Input/IA_Portal_Grab.IA_Portal_Grab'"));
	if ( IA_GrabRef.Object )
	{
		IA_Grab=IA_GrabRef.Object;
	}
}

void UPlayerInputComponent::BeginPlay()
{
	Super::BeginPlay();

	// Input Mapping
	APlayerController* MyController=CastChecked<APlayerController>(Owner->GetController());
	UEnhancedInputLocalPlayerSubsystem* Subsystem=ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(MyController->GetLocalPlayer());
	if ( Subsystem )
	{
		Subsystem->ClearAllMappings();
		Subsystem->AddMappingContext(IMC, 0);
	}
}

void UPlayerInputComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}

void UPlayerInputComponent::InitializeComponent()
{
	Super::InitializeComponent();

	Owner=CastChecked<APlayerCharacter>(GetOwner());
	MoveComp=Owner->GetCharacterMovement();
}

void UPlayerInputComponent::SetupInput(UEnhancedInputComponent* PlayerInputComponent)
{
	UEnhancedInputComponent* InputComp=CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	// Move
	InputComp->BindAction(IA_Move, ETriggerEvent::Triggered, this, &UPlayerInputComponent::OnIAMove);
	// Look
	InputComp->BindAction(IA_Look, ETriggerEvent::Triggered, this, &UPlayerInputComponent::OnIALook);

	// Fire
	InputComp->BindAction(IA_FireLeft, ETriggerEvent::Started, this, &UPlayerInputComponent::OnIAFireLeft);
	InputComp->BindAction(IA_FireRight, ETriggerEvent::Started, this, &UPlayerInputComponent::OnIAFireRight);
	// Grab
	InputComp->BindAction(IA_Grab, ETriggerEvent::Started, this, &UPlayerInputComponent::OnIAGrab);
}

void UPlayerInputComponent::OnIAMove(const FInputActionValue& Value)
{
	const FVector2D InputVec=Value.Get<FVector2D>();

	const FRotator Rotator=Owner->Controller->GetControlRotation();
	const FRotator YawRotator=FRotator(0, Rotator.Yaw, 0);

	const FVector ForwardVec=FRotationMatrix(YawRotator).GetUnitAxis(EAxis::X);
	const FVector RightVec=FRotationMatrix(YawRotator).GetUnitAxis(EAxis::Y);

	Owner->AddMovementInput(ForwardVec, InputVec.Y);
	Owner->AddMovementInput(RightVec, InputVec.X);
}

void UPlayerInputComponent::OnIALook(const FInputActionValue& Value)
{
	const FVector2D InputVec=Value.Get<FVector2D>();

	Owner->AddControllerPitchInput(InputVec.X);
	Owner->AddControllerYawInput(InputVec.Y);
}

void UPlayerInputComponent::OnIAFireLeft(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("FireLeft"));
}

void UPlayerInputComponent::OnIAFireRight(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("FireRight"));
}

void UPlayerInputComponent::OnIAGrab(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Grab"));
}
