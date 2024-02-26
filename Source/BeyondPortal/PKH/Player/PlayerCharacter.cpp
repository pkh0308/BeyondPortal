// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Player/PlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "PlayerInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PKH/UI/CrosshairUIWidget.h"
#include "PKH/Anim/PlayerAnimInstance.h"
#include "PKH/Portal/Portal.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Setting
	GetCharacterMovement()->bOrientRotationToMovement=false;
	bUseControllerRotationRoll=false;
	bUseControllerRotationPitch=false;
	bUseControllerRotationYaw=true;

	// Component
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalRef(TEXT("/Script/Engine.SkeletalMesh'/Game/PKH/Mesh/Player/SKM_Player_Man.SKM_Player_Man'"));
	if( SkeletalRef.Object)
	{
		GetMesh()->SetSkeletalMesh(SkeletalRef.Object);
	}
	GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

	MoveComp=GetCharacterMovement();
	MoveComp->AirControl=0.15f;
	MoveComp->JumpZVelocity=300.0f;

	GunComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunComp"));
	GunComp->SetupAttachment(GetMesh(), "GunSocket");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> GunRef(TEXT("/Script/Engine.StaticMesh'/Game/PKH/Mesh/PortalGun/SM_PortalGun.SM_PortalGun'"));
	if ( GunRef.Object )
	{
		GunComp->SetStaticMesh(GunRef.Object);
		GunComp->SetRelativeScale3D(FVector(5.0f));
	}

	InputComp = CreateDefaultSubobject<UPlayerInputComponent>(TEXT("InputComp"));

	// Camera
	CameraComp=CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(RootComponent);
	CameraComp->AddRelativeLocation(FVector(30, 0, 70));
	CameraComp->bUsePawnControlRotation=true;

	// Portal
	static ConstructorHelpers::FClassFinder<APortal> PortalLClassRef(TEXT("/Game/PKH/Blueprint/BP_PortalL.BP_PortalL_C"));
	if ( PortalLClassRef.Class )
	{
		PortalLClass=PortalLClassRef.Class;
	}
	static ConstructorHelpers::FClassFinder<APortal> PortalRClassRef(TEXT("/Game/PKH/Blueprint/BP_PortalR.BP_PortalR_C"));
	if ( PortalRClassRef.Class )
	{
		PortalRClass=PortalRClassRef.Class;
	}

	// Animation
	static ConstructorHelpers::FClassFinder<UPlayerAnimInstance> AnimInstanceClassRef(TEXT("/Game/PKH/Animation/ABP_Player.ABP_Player_C"));
	if ( AnimInstanceClassRef.Class )
	{
		GetMesh()->SetAnimClass(AnimInstanceClassRef.Class);
	}

	// UI
	static ConstructorHelpers::FClassFinder<UCrosshairUIWidget> CrosshairUIClassRef(TEXT("/Game/PKH/UI/WBP_Crosshair.WBP_Crosshair_C"));
	if( CrosshairUIClassRef.Class )
	{
		CrosshairUIClass=CrosshairUIClassRef.Class;
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// UI
	CrosshairUI=CreateWidget<UCrosshairUIWidget>(GetWorld(), CrosshairUIClass);
	if( CrosshairUI )
	{
		CrosshairUI->AddToViewport();
	}

	// Portal
	PortalLeft = GetWorld()->SpawnActor<APortal>(PortalLClass, FVector(-100), FRotator(0));
	PortalRight = GetWorld()->SpawnActor<APortal>(PortalRClass, FVector(-100), FRotator(0));
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedComp=CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	InputComp->SetupInput(EnhancedComp);
}

void APlayerCharacter::SpawnPortal(const bool IsLeft, const FVector& Location, const FVector& Normal) const
{
	APortal* TargetPortal=IsLeft ? PortalLeft : PortalRight;
	const FVector SpawnLocation=Location + Normal * PortalSpawnOffset;
	FRotator SpawnRotation=Normal.ToOrientationRotator() + FRotator(0, 0, 0);
	
	TargetPortal->SetActorLocation(SpawnLocation);
	TargetPortal->SetActorRotation(SpawnRotation);
	// 캡쳐 회전 초기화
	PortalLeft->ResetCaptureCamera();
	PortalRight->ResetCaptureCamera();

	TargetPortal->Activate(true);

	// Link
	APortal* OtherPortal=IsLeft ? PortalRight : PortalLeft;
	if( nullptr == OtherPortal )
	{
		return;
	}
	if( false == OtherPortal->GetIsActivated() )
	{
		return;
	}

	TargetPortal->LinkPortal(OtherPortal);
	OtherPortal->LinkPortal(TargetPortal);
}

void APlayerCharacter::PlayerCrouch()
{
	if( IsCrouching )
	{
		UnCrouch();
		IsCrouching=false;
	}
	else
	{
		Crouch();
		IsCrouching=true;
	}
}
