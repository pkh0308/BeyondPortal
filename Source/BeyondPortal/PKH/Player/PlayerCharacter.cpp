// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Player/PlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "PlayerInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PKH/UI/CrosshairUIWidget.h"
#include "PKH/Anim/PlayerAnimInstance.h"
#include "PKH/Portal/Portal.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Setting
	
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
	MoveComp->MaxWalkSpeedCrouched=150.0f;
	MoveComp->bUseControllerDesiredRotation=true;
	MoveComp->bOrientRotationToMovement=false;

	GunComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunComp"));
	GunComp->SetupAttachment(GetMesh(), "PortalGunSocket");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> GunRef(TEXT("/Script/Engine.StaticMesh'/Game/PKH/Mesh/PortalGun/SM_PortalGun.SM_PortalGun'"));
	if ( GunRef.Object )
	{
		GunComp->SetStaticMesh(GunRef.Object);
		GunComp->SetRelativeScale3D(FVector(5.0f));
	}

	LightComp=CreateDefaultSubobject<UPointLightComponent>(TEXT("LighComp"));
	LightComp->SetupAttachment(GunComp, TEXT("LightSocket"));
	LightComp->SetAttenuationRadius(8.0f);
	LightComp->SetSourceRadius(5.0f);
	LightComp->SetIntensity(0);

	InputComp = CreateDefaultSubobject<UPlayerInputComponent>(TEXT("InputComp"));

	GunParticleComp=CreateDefaultSubobject<UParticleSystemComponent>(TEXT("GunParticleComp"));
	GunParticleComp->SetupAttachment(GunComp, TEXT("EffectSocket"));
	GunParticleComp->bAutoActivate=false;
	GunParticleComp->bAutoDestroy = false;

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
	PortalExtent = PortalLeft->GetComponentByClass<UBoxComponent>()->GetUnscaledBoxExtent();

	// Particle
	GunParticleComp->SetTemplate(VFX_GrabEffect);
	GunParticleComp->ActivateSystem(false);
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

void APlayerCharacter::PortalGunLightOn(FLinearColor NewColor)
{
	LightComp->SetLightColor(NewColor);
	LightComp->SetIntensity(MaxIntensity);
}

void APlayerCharacter::PortalGunLightOff()
{
	LightComp->SetIntensity(0);
}

void APlayerCharacter::GrabObj(ICanGrab* NewObject)
{
	GrabObject=NewObject;
	if ( GunParticleComp->Template )
	{
		GunParticleComp->ActivateSystem(true);
	}
}

void APlayerCharacter::DropObj()
{
	GrabObject=nullptr;
	if ( GunParticleComp->Template )
	{
		GunParticleComp->ActivateSystem(false);
	}
}

FVector APlayerCharacter::GetGrabPoint() const
{
	const FVector GrabPoint = CameraComp->GetComponentLocation() + FVector(0, 0, 25) + CameraComp->GetForwardVector() * GrabDistance;
	return GrabPoint;
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
