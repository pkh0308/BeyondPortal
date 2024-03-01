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
	FVector ImpactPoint = HitResult.ImpactPoint;
	if( TrySpawnPortal(HitResult, ImpactPoint) )
	{
		Owner->SpawnPortal(true, ImpactPoint, HitResult.ImpactNormal);
		Owner->PortalGunLightOn(FLinearColor::Blue);
	}
	else
	{
		const FRotator EmitterRotation=FRotationMatrix::MakeFromY(HitResult.ImpactNormal).Rotator();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), VFX_PortalLFail, HitResult.ImpactPoint, EmitterRotation);
		UE_LOG(LogTemp, Warning, TEXT("%f %f %f"), HitResult.ImpactPoint.X, HitResult.ImpactPoint.Y, HitResult.ImpactPoint.Z);
	}
}

void UPlayerInputComponent::OnIAFireRight(const FInputActionValue& Value)
{
	FHitResult HitResult;
	FVector ImpactPoint=HitResult.ImpactPoint;
	if ( TrySpawnPortal(HitResult, ImpactPoint) )
	{
		Owner->SpawnPortal(false, ImpactPoint, HitResult.ImpactNormal);
		Owner->PortalGunLightOn(FLinearColor::FromSRGBColor(FColor::Orange));
	}
	else
	{
		const FRotator EmitterRotation =FRotationMatrix::MakeFromY(HitResult.ImpactNormal).Rotator();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), VFX_PortalRFail, HitResult.ImpactPoint, EmitterRotation);
		UE_LOG(LogTemp, Warning, TEXT("%f %f %f"), HitResult.ImpactPoint.X, HitResult.ImpactPoint.Y, HitResult.ImpactPoint.Z);
	}
}

void UPlayerInputComponent::OnIAInteraction(const FInputActionValue& Value)
{
	// Drop Object
	if( Owner->GetGrabObject() )
	{
		Owner->GetGrabObject()->Drop();
		Owner->DropObj();
		return;
	}

	// Check object with line tracing
	FHitResult HitResult;
	UCameraComponent* Camera=Owner->GetCameraComp();
	const FVector StartVec=Camera->GetComponentLocation();
	const FVector EndVec=StartVec + Camera->GetForwardVector() * InteractionDistance;
	DrawDebugLine(GetWorld(), StartVec, EndVec, FColor::Red, false, 3.0f);

	FCollisionQueryParams Param;
	Param.AddIgnoredActor(Owner);
	bool IsHit=GetWorld()->LineTraceSingleByChannel(HitResult, StartVec, EndVec, ECC_Visibility, Param);
	if(false == IsHit)
	{
		return;
	}

	// Grab
	ICanGrab* GrabActor =Cast<ICanGrab>(HitResult.GetActor());
	if(GrabActor)
	{
		GrabActor->Grab(Owner);
		Owner->GrabObj(GrabActor);
		return;
	}

	// Interaction
	IInteractible* InteractibleActor = Cast<IInteractible>(HitResult.GetActor());
	if( InteractibleActor )
	{
		InteractibleActor->DoInteraction();
		return;
	}
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

bool UPlayerInputComponent::TrySpawnPortal(FHitResult& InHitResult, FVector& ImpactPoint) const
{
	// Animation
	PlayFireMontage();

	APlayerCameraManager* CameraManager=UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0); // Should Edit for Network
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
		UE_LOG(LogTemp, Warning, TEXT("No Physical Material"));
		return false;
	}
	if( PM->SurfaceType != PORTAL_SURFACE )
	{
		UE_LOG(LogTemp, Warning, TEXT("Surface is not PORTAL_SURFACE"));
		return false;
	}

	// Adjust Location
	// Only for vertical wall
	if( FMath::Abs(InHitResult.ImpactNormal.X) > 0.99f )
	{
		CalcPortalLocationYZ(ImpactPoint, InHitResult.GetActor()->GetComponentsBoundingBox());
	}
	else if( FMath::Abs(InHitResult.ImpactNormal.Y) > 0.99f )
	{
		CalcPortalLocationXZ(ImpactPoint, InHitResult.GetActor()->GetComponentsBoundingBox()); 
	}
	else if ( FMath::Abs(InHitResult.ImpactNormal.Z) > 0.99f )
	{
		CalcPortalLocationXY(ImpactPoint, InHitResult.GetActor()->GetComponentsBoundingBox()); 
	}
	
	return true;
}

void UPlayerInputComponent::CalcPortalLocationYZ(FVector& ImpactPoint, FBox WallBox) const
{
	const FVector WallExtent=WallBox.GetExtent();
	const FVector WallCenter=WallBox.GetCenter();
	const FVector PortalExtent=Owner->GetPortalExtent();

	const float MaxY=WallCenter.Y + WallExtent.Y - PortalExtent.Y;
	const float MinY=WallCenter.Y - WallExtent.Y + PortalExtent.Y;
	const float MaxZ=WallCenter.Z + WallExtent.Z - PortalExtent.Z;
	const float MinZ=WallCenter.Z - WallExtent.Z + PortalExtent.Z;
	
	ImpactPoint.Y=FMath::Clamp(ImpactPoint.Y, MinY, MaxY);
	ImpactPoint.Z=FMath::Clamp(ImpactPoint.Z, MinZ, MaxZ);
	UE_LOG(LogTemp, Warning, TEXT("%f %f %f"), ImpactPoint.X, ImpactPoint.Y, ImpactPoint.Z);
}

void UPlayerInputComponent::CalcPortalLocationXZ(FVector& ImpactPoint, FBox WallBox) const
{
	const FVector WallExtent=WallBox.GetExtent();
	const FVector WallCenter=WallBox.GetCenter();
	const FVector PortalExtent=Owner->GetPortalExtent();

	const float MaxX=WallCenter.X + WallExtent.X - PortalExtent.Y;
	const float MinX=WallCenter.X - WallExtent.X + PortalExtent.Y;
	const float MaxZ=WallCenter.Z + WallExtent.Z - PortalExtent.Z;
	const float MinZ=WallCenter.Z - WallExtent.Z + PortalExtent.Z;
	ImpactPoint.X=FMath::Clamp(ImpactPoint.X, MinX, MaxX); 
	ImpactPoint.Z=FMath::Clamp(ImpactPoint.Z, MinZ, MaxZ);
	UE_LOG(LogTemp, Warning, TEXT("%f %f %f"), ImpactPoint.X, ImpactPoint.Y, ImpactPoint.Z);
}

void UPlayerInputComponent::CalcPortalLocationXY(FVector& ImpactPoint, FBox WallBox) const
{
	const FVector WallExtent=WallBox.GetExtent();
	const FVector WallCenter=WallBox.GetCenter();
	const FVector PortalExtent=Owner->GetPortalExtent();

	const float MaxY=WallCenter.Y + WallExtent.Y - PortalExtent.Y;
	const float MinY=WallCenter.Y - WallExtent.Y + PortalExtent.Y;
	const float MaxX=WallCenter.X + WallExtent.X - PortalExtent.Z;
	const float MinX=WallCenter.X - WallExtent.X + PortalExtent.Z;
	
	ImpactPoint.X=FMath::Clamp(ImpactPoint.X, MinX, MaxX);
	ImpactPoint.Y=FMath::Clamp(ImpactPoint.Y, MinY, MaxY);
	UE_LOG(LogTemp, Warning, TEXT("%f %f %f"), ImpactPoint.X, ImpactPoint.Y, ImpactPoint.Z);
}
