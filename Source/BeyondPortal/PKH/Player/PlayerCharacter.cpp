// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Player/PlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "PlayerInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PKH/UI/CrosshairUIWidget.h"
#include "PKH/Anim/PlayerAnimInstance.h"
#include "PKH/Portal/Portal.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "EngineUtils.h"
#include "PKH/Props/GunActor.h"
#include "SEB/Barrier.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Replicate
	bReplicates=true;
	NetUpdateFrequency=100.0f;

	// Rotation Setting
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

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnPlayerBeginOverlap);

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
	GunParticleComp->SetRelativeScale3D(FVector(0.05f));
	GunParticleComp->bAutoActivate = false;
	GunParticleComp->bAutoDestroy = false;

	// Camera
	CameraComp=CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(RootComponent);
	CameraComp->AddRelativeLocation(FVector(17, 7, 28));
	CameraComp->bUsePawnControlRotation=true;

	// Gun Actor
	static ConstructorHelpers::FClassFinder<AGunActor> GunActorClassRef(TEXT("/Game/PKH/Blueprint/Props/BP_GunActor.BP_GunActor_C"));
	if ( GunActorClassRef.Class )
	{
		GunActorClass=GunActorClassRef.Class;
	}

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

	// 위치 조정(클라)
	if( false == HasAuthority() )
	{
		//RPC_SetPlayerLocation(this);
	}

	// Gun Actor
	GunActor = GetWorld()->SpawnActor<AGunActor>(GunActorClass);
	GunActor->SetActive(false);

	// Respawn
	RespawnLocation=GetActorLocation();

	// UI
	CrosshairUI=CreateWidget<UCrosshairUIWidget>(GetWorld(), CrosshairUIClass);
	if( CrosshairUI )
	{
		CrosshairUI->AddToViewport();
	}

	// Portal
	PortalLeft=GetWorld()->SpawnActor<APortal>(PortalLClass, FVector(-100), FRotator(0));
	PortalRight=GetWorld()->SpawnActor<APortal>(PortalRClass, FVector(-100), FRotator(0));
	PortalExtent=PortalLeft->GetComponentByClass<UBoxComponent>()->GetUnscaledBoxExtent();

	if( HasAuthority() )
	{
		PortalLeft->SetOwner(GetController());
		PortalRight->SetOwner(GetController());
		PortalLeft->SetCapturePlayer(this);
		PortalRight->SetCapturePlayer(this);
	}
	else
	{
		RPC_Server_InitPortal();

		PortalLeft->SetCapturePlayer(this);
		PortalRight->SetCapturePlayer(this);

		APlayerCharacter* OtherPlayer=Cast< APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		if ( nullptr == OtherPlayer )
		{
			UE_LOG(LogTemp, Log, TEXT("Set Capture Player Fail - idx 0 Player not exist"));
			return;
		}
		OtherPlayer->PortalLeft->SetCapturePlayer(this);
		OtherPlayer->PortalRight->SetCapturePlayer(this);
	}

	// Particle
	GunParticleComp->SetTemplate(VFX_GrabEffect);
	GunParticleComp->SetActive(false);
}

void APlayerCharacter::RPC_Server_InitPortal_Implementation()
{
	APlayerCharacter* OtherPlayer=Cast< APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 1));
	if ( nullptr == OtherPlayer )
	{
		UE_LOG(LogTemp, Warning, TEXT("[RPC_Server_InitPortal] There is no OtherPlayer"));
		return;
	}

	APlayerController* OtherController=UGameplayStatics::GetPlayerController(GetWorld(), 1);
	if( nullptr == OtherController )
	{
		UE_LOG(LogTemp, Warning, TEXT("[RPC_Server_InitPortal] There is no OtherController"));
		return;
	}

	OtherPlayer->PortalLeft->SetOwner(OtherController);
	OtherPlayer->PortalRight->SetOwner(OtherController);
	OtherPlayer->PortalLeft->SetCapturePlayer(this);
	OtherPlayer->PortalRight->SetCapturePlayer(this);
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
	Spawn(IsLeft, Location, Normal);
}

void APlayerCharacter::Spawn(const bool IsLeft, const FVector& Location, const FVector& Normal) const
{
	APortal* TargetPortal=IsLeft ? PortalLeft : PortalRight;
	const FVector SpawnLocation=Location + Normal * PortalSpawnOffset;
	const FRotator SpawnRotation=Normal.ToOrientationRotator();

	if( HasAuthority() )
	{
		TargetPortal->SetActorLocation(SpawnLocation);
		TargetPortal->SetActorRotation(SpawnRotation);
	}
	else
	{
		RPC_Server_SpawnPortal(TargetPortal, SpawnLocation, SpawnRotation);
	}
	TargetPortal->Activate(true);
	CrosshairUI->PortalUI_Empty(IsLeft);

	// Link
	APortal* OtherPortal=IsLeft ? PortalRight : PortalLeft;
	if ( nullptr == OtherPortal )
	{
		return;
	}
	if ( false == OtherPortal->GetIsActivated() )
	{
		return;
	}

	TargetPortal->LinkPortal(OtherPortal);
	OtherPortal->LinkPortal(TargetPortal);
}

void APlayerCharacter::RPC_Server_SpawnPortal_Implementation(APortal* TargetPortal, const FVector& NewLocation, const FRotator& NewRotation) const
{
	TargetPortal->SetActorLocation(NewLocation);
	TargetPortal->SetActorRotation(NewRotation);
}

bool APlayerCharacter::IsOverlapPortal(bool IsLeft, FVector TargetCenter)
{
	APortal* TargetPortal=IsLeft ? PortalLeft : PortalRight;
	if( false == TargetPortal->GetIsActivated())
	{
		return false;
	}

	const FVector TargetBox=TargetPortal->GetComponentsBoundingBox().GetExtent();
	const float TargetMaxX=TargetCenter.X + TargetBox.X;
	const float TargetMinX=TargetCenter.X - TargetBox.X;
	const float TargetMaxY=TargetCenter.Y + TargetBox.Y;
	const float TargetMinY=TargetCenter.Y - TargetBox.Y;
	const float TargetMaxZ=TargetCenter.Z + TargetBox.Z;
	const float TargetMinZ=TargetCenter.Z - TargetBox.Z;

	const APortal* OtherPortal=IsLeft ? PortalRight : PortalLeft;
	const FVector LinkedCenter=OtherPortal->GetActorLocation();
	const FVector LinkedBox=OtherPortal->GetComponentsBoundingBox().GetExtent();
	const float LinkedMaxX=LinkedCenter.X + LinkedBox.X;
	const float LinkedMinX=LinkedCenter.X - LinkedBox.X;
	const float LinkedMaxY=LinkedCenter.Y + LinkedBox.Y;
	const float LinkedMinY=LinkedCenter.Y - LinkedBox.Y;
	const float LinkedMaxZ=LinkedCenter.Z + LinkedBox.Z;
	const float LinkedMinZ=LinkedCenter.Z - LinkedBox.Z;

	const bool XOverlap = (TargetCenter.X < LinkedCenter.X && TargetMaxX >= LinkedMinX) || (TargetCenter.X > LinkedCenter.X && TargetMinX <= LinkedMaxX);
	const bool YOverlap = (TargetCenter.Y < LinkedCenter.Y && TargetMaxY >= LinkedMinY) || (TargetCenter.Y > LinkedCenter.Y && TargetMinY <= LinkedMaxY);
	const bool ZOverlap = (TargetCenter.Z < LinkedCenter.Z && TargetMaxZ >= LinkedMinZ) || (TargetCenter.Z > LinkedCenter.Z && TargetMinZ <= LinkedMaxZ);

	return XOverlap && YOverlap && ZOverlap;
}

void APlayerCharacter::ResetAllPortals()
{
	PortalLeft->Activate(false);
	PortalRight->Activate(false);

	// UI
	CrosshairUI->PortalUI_Fill(true);
	CrosshairUI->PortalUI_Fill(false);
}

void APlayerCharacter::ChangeVelocity(const FVector& NewDirection, const float Multiplier)
{
	const float CurVelocitySize=GetCharacterMovement()->Velocity.Size();
	const float Offset=FMath::Clamp(FMath::Atan(CurVelocitySize / 400), 0.9f, 1.15f);
	// Velocity가 클수록 증폭
	const float NewVelocity=CurVelocitySize * Multiplier * 1.15f;
	GetCharacterMovement()->Velocity=NewDirection * NewVelocity;
}

void APlayerCharacter::OnPlayerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABarrier* Barrier=Cast<ABarrier>(OtherActor);
	if( Barrier )
	{
		const FVector NewLocation=Barrier->GetActorLocation();
		if( RespawnLocation != NewLocation )
		{
			RespawnLocation=NewLocation;
		}
	}
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

FVector APlayerCharacter::GetCameraLocation() const
{
	return CameraComp->GetComponentLocation();
}

void APlayerCharacter::GrabObj(ICanGrab* NewObject)
{
	GrabObject=NewObject;
	if ( GunParticleComp->Template )
	{
		GunParticleComp->SetActive(true);
	}

	CrosshairUI->SetVisibility(ESlateVisibility::Hidden); UE_LOG(LogTemp, Warning, TEXT("Player Grab"));
}

void APlayerCharacter::DropObj()
{
	GrabObject=nullptr;
	if ( GunParticleComp->Template )
	{
		GunParticleComp->SetActive(false);
	}

	CrosshairUI->SetVisibility(ESlateVisibility::Visible); UE_LOG(LogTemp, Warning, TEXT("Player Drop"));
}

FVector APlayerCharacter::GetGrabPoint() const
{
	const FVector GrabPoint = CameraComp->GetComponentLocation() + FVector(0, 0, 25) + CameraComp->GetForwardVector() * GrabDistance;
	return GrabPoint;
}

void APlayerCharacter::Respawn()
{
	IsDead=false;

	SetActorLocation(RespawnLocation);

	// Animation
	UPlayerAnimInstance* Anim=Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	if ( Anim )
	{
		Anim->StopAllMontages(0);
	}

	// Gun
	GunComp->SetVisibility(true);
	GunActor->SetActive(false);

	// Camera
	CameraComp->SetRelativeLocation(CameraDefaultLocation);
	GetMesh()->SetOwnerNoSee(true);
}

void APlayerCharacter::OnDie()
{
	IsDead=true;

	// Animation
	UPlayerAnimInstance* Anim=Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	if( Anim )
	{
		Anim->PlayMontage_Dead();
	}

	// Gun 
	GunComp->SetVisibility(false);
	GunActor->SetActive(true);
	GunActor->SetActorLocation(GunComp->GetComponentLocation());
	GunActor->SetActorRotation(GunComp->GetComponentRotation());

	// Camera
	CameraComp->SetRelativeLocation(CameraOnDieLocation);
	GetMesh()->SetOwnerNoSee(false);

	// Respawn
	FTimerHandle RespawnHandle;
	GetWorldTimerManager().SetTimer(RespawnHandle, this, &APlayerCharacter::Respawn, 3.0f, false);
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

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerCharacter, Net_ControlRotation);
}

void APlayerCharacter::OnRep_ControlRotationChanged()
{
	GetController()->SetControlRotation(Net_ControlRotation);
}

void APlayerCharacter::RPC_SetPlayerLocation_Implementation(ACharacter* ClientPlayer)
{
	ClientPlayer->SetActorLocation(FVector(60, -460, 73));
}
