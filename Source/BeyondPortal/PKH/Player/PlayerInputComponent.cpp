// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Player/PlayerInputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerCharacter.h"
#include "InputMappingContext.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PKH/Anim/PlayerAnimInstance.h"
#include "PKH/Interface/CanGrab.h"
#include "PKH/Interface/Interactible.h"

#define PORTAL_SURFACE EPhysicalSurface::SurfaceType1

UPlayerInputComponent::UPlayerInputComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

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
	// Emotion
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_EmotionRef(TEXT("/Script/EnhancedInput.InputAction'/Game/PKH/Input/IA_Portal_Emotion.IA_Portal_Emotion'"));
	if ( IA_EmotionRef.Object )
	{
		IA_Emotion=IA_EmotionRef.Object;
	}
	// Target
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_TargetRef(TEXT("/Script/EnhancedInput.InputAction'/Game/PKH/Input/IA_Portal_Target.IA_Portal_Target'"));
	if ( IA_TargetRef.Object )
	{
		IA_Target=IA_TargetRef.Object;
	}
	// Voice
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_VoiceRef(TEXT("/Script/EnhancedInput.InputAction'/Game/PKH/Input/IA_Portal_Voice.IA_Portal_Voice'"));
	if ( IA_VoiceRef.Object )
	{
		IA_Voice=IA_VoiceRef.Object;
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

	// Emotion
	InputComp->BindAction(IA_Emotion, ETriggerEvent::Started, this, &UPlayerInputComponent::OnIAEmotionUIOn);
	InputComp->BindAction(IA_Emotion, ETriggerEvent::Completed, this, &UPlayerInputComponent::OnIAEmotionUIOff);

	// Target
	InputComp->BindAction(IA_Target, ETriggerEvent::Started, this, &UPlayerInputComponent::OnIATarget);

	// Voice
	InputComp->BindAction(IA_Voice, ETriggerEvent::Started, this, &UPlayerInputComponent::OnIABeginVoice);
	InputComp->BindAction(IA_Voice, ETriggerEvent::Completed, this, &UPlayerInputComponent::OnIAEndVoice);
}

void UPlayerInputComponent::OnIAMove(const FInputActionValue& Value)
{
	if(Owner->IsPlayerDead() || Owner->GetIsShowingEmotion())
	{
		return;
	}

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
	if ( Owner->IsPlayerDead() || Owner->GetIsShowingEmotion() )
	{
		return;
	}

	const FVector2D InputVec=Value.Get<FVector2D>();

	Owner->AddControllerPitchInput(InputVec.X * MouseSensitivity);
	Owner->AddControllerYawInput(InputVec.Y * MouseSensitivity);

	if(Owner->IsLocallyControlled() )
	{
		Owner->Look(InputVec.X * MouseSensitivity, InputVec.Y * MouseSensitivity);
	}
}

void UPlayerInputComponent::OnIAJump(const FInputActionValue& Value)
{
	if ( Owner->IsPlayerDead() || Owner->GetIsShowingEmotion() )
	{
		return;
	}

	Owner->Jump();
}

void UPlayerInputComponent::OnIACrouch(const FInputActionValue& Value)
{
	if ( Owner->IsPlayerDead() || Owner->GetIsShowingEmotion() )
	{
		return;
	}

	Owner->PlayerCrouch();
}

void UPlayerInputComponent::OnIAFireLeft(const FInputActionValue& Value)
{
	if ( Owner->IsPlayerDead() || Owner->GetIsShowingEmotion() )
	{
		return;
	}
	if(Owner->GetGrabObject())
	{
		return;
	}

	FHitResult HitResult;
	FVector ImpactPoint = HitResult.ImpactPoint;
	if( TrySpawnPortal(HitResult, ImpactPoint, true) )
	{
		Owner->SpawnPortal(true, ImpactPoint, HitResult.ImpactNormal);
	}
	else
	{
		const FRotator EmitterRotation=FRotationMatrix::MakeFromY(HitResult.ImpactNormal).Rotator();
		Owner->SpawnFail(VFX_PortalLFail, HitResult.ImpactPoint, EmitterRotation);
	}
	Owner->PortalGunLightOn(true);
}

void UPlayerInputComponent::OnIAFireRight(const FInputActionValue& Value)
{
	if ( Owner->IsPlayerDead() || Owner->GetIsShowingEmotion() )
	{
		return;
	}
	if (Owner->GetGrabObject())
	{
		return;
	}

	FHitResult HitResult;
	FVector ImpactPoint=HitResult.ImpactPoint;
	if ( TrySpawnPortal(HitResult, ImpactPoint, false) )
	{
		Owner->SpawnPortal(false, ImpactPoint, HitResult.ImpactNormal);
	}
	else
	{
		const FRotator EmitterRotation =FRotationMatrix::MakeFromY(HitResult.ImpactNormal).Rotator();
		Owner->SpawnFail(VFX_PortalRFail, HitResult.ImpactPoint, EmitterRotation);
	}
	Owner->PortalGunLightOn(false);
}

void UPlayerInputComponent::OnIAInteraction(const FInputActionValue& Value)
{
	if ( Owner->IsPlayerDead() || Owner->GetIsShowingEmotion() )
	{
		return;
	}

	if(Owner->IsLocallyControlled())
	{
		Owner->RPC_Server_Interaction(InteractionDistance);
	}
}

void UPlayerInputComponent::OnIAEmotionUIOn(const FInputActionValue& Value)
{
	if ( Owner->IsPlayerDead() || Owner->GetIsShowingEmotion() )
	{
		return;
	}

	Owner->SetEmotionUI(true); 
}

void UPlayerInputComponent::OnIAEmotionUIOff(const FInputActionValue& Value)
{
	if ( Owner->IsPlayerDead() )
	{
		return;
	}

	Owner->SetEmotionUI(false);
}

void UPlayerInputComponent::OnIATarget(const FInputActionValue& Value)
{
	if ( Owner->IsPlayerDead() || Owner->GetIsShowingEmotion() )
	{
		return;
	}

	Owner->SetTargetUI();
}

void UPlayerInputComponent::OnIABeginVoice(const FInputActionValue& Value)
{
	Owner->VoiceChat(true); UE_LOG(LogTemp, Warning, TEXT("Voice Start"));
}

void UPlayerInputComponent::OnIAEndVoice(const FInputActionValue& Value)
{
	Owner->VoiceChat(false); UE_LOG(LogTemp, Warning, TEXT("Voice End"));
}

void UPlayerInputComponent::PlayFireMontage() const
{
	UAnimInstance* AnimInstance=Owner->GetMesh()->GetAnimInstance();
	if ( nullptr == AnimInstance )
	{
		return;
	}

	UPlayerAnimInstance* PlayerAnim=Cast<UPlayerAnimInstance>(AnimInstance);
	if ( nullptr == PlayerAnim )
	{
		return;
	}

	PlayerAnim->PlayMontage_Fire();
}

bool UPlayerInputComponent::TrySpawnPortal(FHitResult& InHitResult, FVector& ImpactPoint, bool IsLeft) const
{
	// Animation
	PlayFireMontage();

	// Line Trace
	APlayerCameraManager* CameraManager=UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	const FVector StartVec=CameraManager->GetCameraLocation();
	const FVector EndVec=StartVec + CameraManager->GetActorForwardVector() * 10000;

	FCollisionQueryParams Param;
	Param.AddIgnoredActor(Owner);
	bool IsHit=GetWorld()->LineTraceSingleByChannel(InHitResult, StartVec, EndVec, ECC_Visibility, Param);
	if ( false == IsHit )
	{
		return false;
	}
	ImpactPoint=InHitResult.ImpactPoint;

	// Check surface
	const auto MI =InHitResult.GetComponent()->GetMaterial(0);
	if( nullptr == MI )
	{
		return false;
	}
	const auto PM = MI->GetPhysicalMaterial();
	if( nullptr == PM )
	{
		//UE_LOG(LogTemp, Log, TEXT("No Physical Material"));
		return false;
	}
	if( PM->SurfaceType != PORTAL_SURFACE )
	{
		//UE_LOG(LogTemp, Log, TEXT("Surface is not PORTAL_SURFACE"));
		return false;
	}

	// Adjust Location
	// Only for vertical wall
	if( FMath::Abs(InHitResult.ImpactNormal.X) > 0.99f )
	{
		if(false == CalcPortalLocationYZ(ImpactPoint, InHitResult.GetActor()->GetComponentsBoundingBox()))
		{
			return false;
		}
	}
	if( FMath::Abs(InHitResult.ImpactNormal.Y) > 0.99f )
	{
		if(false == CalcPortalLocationXZ(ImpactPoint, InHitResult.GetActor()->GetComponentsBoundingBox()))
		{
			return false;
		}
	}
	if ( FMath::Abs(InHitResult.ImpactNormal.Z) > 0.99f )
	{
		if(false == CalcPortalLocationXY(ImpactPoint, InHitResult.GetActor()->GetComponentsBoundingBox()) )
		{
			return false;
		}
	}
	//CalcPortalLocation(ImpactPoint, InHitResult.ImpactNormal, InHitResult.GetActor()->GetComponentsBoundingBox());

	// Prevent portal overlap
	if ( Owner->IsOverlapPortal(IsLeft, ImpactPoint) )
	{
		return false;
	}

	return true;
}

bool UPlayerInputComponent::CalcPortalLocationYZ(FVector& ImpactPoint, const FBox& WallBox) const
{
	const FVector WallExtent=WallBox.GetExtent();
	const FVector WallCenter=WallBox.GetCenter();
	FVector PortalExtent=Owner->GetPortalExtent();
	PortalExtent.X = -1;

	const float MaxX=WallCenter.X + WallExtent.X - PortalExtent.X;
	const float MinX=WallCenter.X - WallExtent.X + PortalExtent.X;
	const float MaxY=WallCenter.Y + WallExtent.Y - PortalExtent.Y;
	const float MinY=WallCenter.Y - WallExtent.Y + PortalExtent.Y;
	const float MaxZ=WallCenter.Z + WallExtent.Z - PortalExtent.Z;
	const float MinZ=WallCenter.Z - WallExtent.Z + PortalExtent.Z;

	if( MinX > MaxX ) return false;
	if( MinY > MaxY ) return false;
	if( MinZ > MaxZ ) return false;

	ImpactPoint.X=FMath::Clamp(ImpactPoint.X, MinX, MaxX);
	ImpactPoint.Y=FMath::Clamp(ImpactPoint.Y, MinY, MaxY);
	ImpactPoint.Z=FMath::Clamp(ImpactPoint.Z, MinZ, MaxZ);
	//UE_LOG(LogTemp, Warning, TEXT("%f %f %f"), ImpactPoint.X, ImpactPoint.Y, ImpactPoint.Z);
	return true;
}

bool UPlayerInputComponent::CalcPortalLocationXZ(FVector& ImpactPoint, const FBox& WallBox) const
{
	const FVector WallExtent=WallBox.GetExtent();
	const FVector WallCenter=WallBox.GetCenter();
	FVector PortalExtent=Owner->GetPortalExtent();
	PortalExtent.X=-1;

	const float MaxX=WallCenter.X + WallExtent.X - PortalExtent.Y;
	const float MinX=WallCenter.X - WallExtent.X + PortalExtent.Y;
	const float MaxY=WallCenter.Y + WallExtent.Y - PortalExtent.X;
	const float MinY=WallCenter.Y - WallExtent.Y + PortalExtent.X;
	const float MaxZ=WallCenter.Z + WallExtent.Z - PortalExtent.Z;
	const float MinZ=WallCenter.Z - WallExtent.Z + PortalExtent.Z;

	if ( MinX > MaxX ) return false;
	if ( MinY > MaxY ) return false;
	if ( MinZ > MaxZ ) return false;

	ImpactPoint.X=FMath::Clamp(ImpactPoint.X, MinX, MaxX);
	ImpactPoint.Y=FMath::Clamp(ImpactPoint.Y, MinY, MaxY);
	ImpactPoint.Z=FMath::Clamp(ImpactPoint.Z, MinZ, MaxZ);
	//UE_LOG(LogTemp, Warning, TEXT("%f %f %f"), ImpactPoint.X, ImpactPoint.Y, ImpactPoint.Z);
	return true;
}

bool UPlayerInputComponent::CalcPortalLocationXY(FVector& ImpactPoint, const FBox& WallBox) const
{
	const FVector WallExtent=WallBox.GetExtent();
	const FVector WallCenter=WallBox.GetCenter();
	FVector PortalExtent=Owner->GetPortalExtent();
	PortalExtent.X=-1;

	const float MaxX=WallCenter.X + WallExtent.X - PortalExtent.Z;
	const float MinX=WallCenter.X - WallExtent.X + PortalExtent.Z;
	const float MaxY=WallCenter.Y + WallExtent.Y - PortalExtent.Y;
	const float MinY=WallCenter.Y - WallExtent.Y + PortalExtent.Y;
	const float MaxZ=WallCenter.Z + WallExtent.Z - PortalExtent.X;
	const float MinZ=WallCenter.Z - WallExtent.Z + PortalExtent.X;

	if ( MinX > MaxX ) return false;
	if ( MinY > MaxY ) return false;
	if ( MinZ > MaxZ ) return false;
	
	ImpactPoint.X=FMath::Clamp(ImpactPoint.X, MinX, MaxX);
	ImpactPoint.Y=FMath::Clamp(ImpactPoint.Y, MinY, MaxY);
	ImpactPoint.Z=FMath::Clamp(ImpactPoint.Z, MinZ, MaxZ);
	//UE_LOG(LogTemp, Warning, TEXT("%f %f %f"), ImpactPoint.X, ImpactPoint.Y, ImpactPoint.Z);
	return true;
}

void UPlayerInputComponent::CalcPortalLocation(FVector& ImpactPoint, const FVector& ImpactNormal, const FBox& WallBox) const
{
	const FVector WallExtent=WallBox.GetExtent();
	const FVector WallCenter=WallBox.GetCenter();
	FVector PortalExtent=Owner->GetPortalExtent();
	PortalExtent.X=-1;

	float MaxX=WallCenter.X + WallExtent.X - PortalExtent.X;
	float MinX=WallCenter.X - WallExtent.X + PortalExtent.X;
	float MaxY=WallCenter.Y + WallExtent.Y - PortalExtent.Y;
	float MinY=WallCenter.Y - WallExtent.Y + PortalExtent.Y;
	float MaxZ=WallCenter.Z + WallExtent.Z - PortalExtent.Z;
	float MinZ=WallCenter.Z - WallExtent.Z + PortalExtent.Z;

	FVector UpperLeft=FVector(MaxX, MinY, MaxZ);
	FVector UpperRight=FVector(MaxX, MaxY, MaxZ);
	FVector LowerLeft=FVector(MaxX, MinY, MinZ);
	FVector LowerRight=FVector(MaxX, MaxY, MinZ);

	const FVector RotationVec=ImpactNormal * -1;
	const FMatrix RotationMatrix=FRotationMatrix::MakeFromX(RotationVec);
	UpperLeft = RotationMatrix.TransformVector(UpperLeft);
	UpperRight = RotationMatrix.TransformVector(UpperRight);
	LowerLeft = RotationMatrix.TransformVector(LowerLeft);
	LowerRight = RotationMatrix.TransformVector(LowerRight);

	MaxX = FMath::Max(UpperLeft.X, UpperRight.X);
	MinX = FMath::Min(UpperLeft.X, UpperRight.X);
	MaxY = FMath::Max(UpperLeft.Y, UpperRight.Y);
	MinY = FMath::Min(UpperLeft.Y, UpperRight.Y);
	MaxZ = FMath::Max(UpperLeft.Z, LowerLeft.Z);
	MinZ = FMath::Min(UpperLeft.Z, LowerLeft.Z);

	ImpactPoint.X=FMath::Clamp(ImpactPoint.X, MinX, MaxX);
	ImpactPoint.Y=FMath::Clamp(ImpactPoint.Y, MinY, MaxY);
	ImpactPoint.Z=FMath::Clamp(ImpactPoint.Z, MinZ, MaxZ);
	//UE_LOG(LogTemp, Warning, TEXT("%f %f %f"), ImpactPoint.X, ImpactPoint.Y, ImpactPoint.Z);
}
