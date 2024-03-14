// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Player/PlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "PlayerInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
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
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "PKH/Props/GrabCube.h"
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

	// PhysicsHandle
	PhysicsHandleComp=CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandleComp"));

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

	// Sound
	static ConstructorHelpers::FObjectFinder<USoundBase> SFX_PortalLeftRef(TEXT("/Script/Engine.SoundWave'/Game/PKH/Sound/SFX_PortalLeft.SFX_PortalLeft'"));
	if ( SFX_PortalLeftRef.Object )
	{
		SFX_PortalLeft=SFX_PortalLeftRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> SFX_PortalRightRef(TEXT("/Script/Engine.SoundWave'/Game/PKH/Sound/SFX_PortalRight.SFX_PortalRight'"));
	if ( SFX_PortalRightRef.Object )
	{
		SFX_PortalRight=SFX_PortalRightRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> SFX_PortalOutWeakRef(TEXT("/Script/Engine.SoundWave'/Game/PKH/Sound/SFX_PortalOutWeak.SFX_PortalOutWeak'"));
	if ( SFX_PortalOutWeakRef.Object )
	{
		SFX_PortalOutWeak=SFX_PortalOutWeakRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> SFX_PortalOutStrongRef(TEXT("/Script/Engine.SoundWave'/Game/PKH/Sound/SFX_PortalOutStrong.SFX_PortalOutStrong'"));
	if ( SFX_PortalOutStrongRef.Object )
	{
		SFX_PortalOutStrong=SFX_PortalOutStrongRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> SFX_PortalFailRef(TEXT("/Script/Engine.SoundWave'/Game/PKH/Sound/SFX_PortalFail.SFX_PortalFail'"));
	if ( SFX_PortalFailRef.Object )
	{
		SFX_PortalFail=SFX_PortalFailRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> SFX_GrabRef(TEXT("/Script/Engine.SoundWave'/Game/PKH/Sound/SFX_Grab.SFX_Grab'"));
	if ( SFX_GrabRef.Object )
	{
		SFX_Grab=SFX_GrabRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> SFX_GrabLoopRef(TEXT("/Script/Engine.SoundWave'/Game/PKH/Sound/SFX_GrabLoop.SFX_GrabLoop'"));
	if ( SFX_GrabLoopRef.Object )
	{
		SFX_GrabLoop=SFX_GrabLoopRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> SFX_DropRef(TEXT("/Script/Engine.SoundWave'/Game/PKH/Sound/SFX_Drop.SFX_Drop'"));
	if ( SFX_DropRef.Object )
	{
		SFX_Drop=SFX_DropRef.Object;
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Set Network Owner
	if( HasAuthority() )
	{
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([this]()
		{
			SetOwner(GetController());
			ACharacter* OtherPlayer = UGameplayStatics::GetPlayerCharacter(GetWorld(), 1); 
			if( OtherPlayer )
			{
				OtherPlayer->SetOwner(UGameplayStatics::GetPlayerController(GetWorld(), 1));
			}
		}), 3.0f, false);
	}

	// Gun Actor
	//GunActor = GetWorld()->SpawnActor<AGunActor>(GunActorClass);
	//GunActor->SetActive(false);

	// Respawn
	RespawnLocation=GetActorLocation();

	// UI
	if(IsLocallyControlled() )
	{
		CrosshairUI=CreateWidget<UCrosshairUIWidget>(GetWorld(), CrosshairUIClass);
		if ( CrosshairUI )
		{
			CrosshairUI->AddToViewport();
		}
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
		PortalLeft->SetIsLeft(true);
		PortalRight->SetIsLeft(false);
	}
	else
	{
		PortalLeft->SetCapturePlayer(this);
		PortalRight->SetCapturePlayer(this);
		PortalLeft->SetIsLeft(true);
		PortalRight->SetIsLeft(false);
	}

	// Particle
	GunParticleComp->SetTemplate(VFX_GrabEffect);
	GunParticleComp->SetActive(false);

	// Sound
	GunSoundComp=UGameplayStatics::SpawnSound2D(GetWorld(), SFX_GrabLoop, 1.0f);
	GunSoundComp->Stop();
	GunSoundComp->bAutoDestroy=false;
}

void APlayerCharacter::RPC_Server_InitPortal_Implementation()
{
	APlayerCharacter* OtherPlayer=Cast< APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
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

	// Grab
	if(nullptr != GrabObject)
	{
		TickGrab();
	}
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
	const FVector SpawnLocation=Location + Normal * PortalSpawnOffset;
	const FRotator SpawnRotation=Normal.ToOrientationRotator();
	
	if( HasAuthority() )
	{
		RPC_Multi_SpawnPortal(IsLeft, SpawnLocation, SpawnRotation, Normal);
	}
	else
	{
		RPC_Server_SpawnPortal(IsLeft, SpawnLocation, SpawnRotation, Normal);
	}
	if(CrosshairUI)
	{
		CrosshairUI->PortalUI_Empty(IsLeft);
	}

	//Sound
	if(IsLocallyControlled())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), IsLeft ? SFX_PortalLeft : SFX_PortalRight, 1.0f);
	}

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
	
	if( HasAuthority() )
	{
		RPC_Multi_LinkPortal(); 
	}
	else
	{
		RPC_Server_LinkPortal();
	}
}

void APlayerCharacter::RPC_Server_SpawnPortal_Implementation(bool IsLeft, const FVector& NewLocation, FRotator NewRotation, const FVector& Normal) const
{
	RPC_Multi_SpawnPortal(IsLeft, NewLocation, NewRotation, Normal);
}

void APlayerCharacter::RPC_Multi_SpawnPortal_Implementation(bool IsLeft, const FVector& NewLocation, FRotator NewRotation, const FVector& Normal) const
{
	APortal* TargetPortal=IsLeft ? PortalLeft : PortalRight;

	// 바닥이나 천장
	if( FMath::Abs(Normal.Z) > 0.9f )
	{
		NewRotation.Roll = GetActorRotation().Yaw * -1;
		NewRotation.Yaw=180;
	}

	TargetPortal->SetActorLocation(NewLocation);
	TargetPortal->SetActorRotation(NewRotation);
	TargetPortal->Activate(true);
}

void APlayerCharacter::RPC_Server_LinkPortal_Implementation() const
{
	RPC_Multi_LinkPortal();
}

void APlayerCharacter::RPC_Multi_LinkPortal_Implementation() const
{
	PortalLeft->LinkPortal(PortalRight);
	PortalRight->LinkPortal(PortalLeft);
}

// Spawn Fail
void APlayerCharacter::SpawnFail(UParticleSystem* TargetVFX, const FVector& NewLocation, const FRotator& NewRotation) const
{
	if( HasAuthority() )
	{
		RPC_Multi_SpawnFail(TargetVFX, NewLocation, NewRotation);
	}
	else
	{
		RPC_Server_SpawnFail(TargetVFX, NewLocation, NewRotation);
	}

	if(IsLocallyControlled())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), SFX_PortalFail, 1.0f);
	}
}

void APlayerCharacter::Look(float PItchInput, float YawInput)
{
	if ( HasAuthority() )
	{
		AddControllerPitchInput(PItchInput);
		AddControllerYawInput(YawInput);
	}
	else
	{
		RPC_Server_Look(PItchInput, YawInput);
	}
}

void APlayerCharacter::RPC_Server_Look_Implementation(float PItchInput, float YawInput)
{
	RPC_Multi_Look(PItchInput, YawInput);
}

void APlayerCharacter::RPC_Multi_Look_Implementation(float PItchInput, float YawInput)
{
	AddControllerPitchInput(PItchInput);
	AddControllerYawInput(YawInput);

	CameraComp->SetWorldRotation(GetController()->GetControlRotation());
}

void APlayerCharacter::RPC_Server_SpawnFail_Implementation(UParticleSystem* TargetVFX, const FVector& NewLocation, const FRotator& NewRotation) const
{
	RPC_Multi_SpawnFail(TargetVFX, NewLocation, NewRotation);
}

void APlayerCharacter::RPC_Multi_SpawnFail_Implementation(UParticleSystem* TargetVFX, const FVector& NewLocation, const FRotator& NewRotation) const
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TargetVFX, NewLocation, NewRotation);
}

void APlayerCharacter::TickGrab()
{
	if ( HasAuthority() )
	{
		PhysicsHandleComp->SetTargetLocation(GetGrabPoint());

		// Check Velocity
		AGrabCube* Cube=Cast<AGrabCube>(GrabObject);
		if(Cube)
		{
			Cube->VelocityCheck();
		}
	}
}

void APlayerCharacter::GrabObj(ICanGrab* NewObject, UPrimitiveComponent* TargetComp)
{
	GrabObject=NewObject;
	PhysicsHandleComp->GrabComponentAtLocationWithRotation(TargetComp, FName(), TargetComp->GetComponentLocation(), TargetComp->GetComponentRotation());
	// Particle
	if ( GunParticleComp->Template )
	{
		GunParticleComp->SetActive(true);
	}
	// UI
	if(CrosshairUI )
	{
		CrosshairUI->SetVisibility(ESlateVisibility::Hidden);
	}
	// Sound
	if(IsLocallyControlled())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), SFX_Grab, 1.0f);
		if(GunSoundComp)
		{
			GunSoundComp->Play();
		}
	}
}

void APlayerCharacter::DropObj()
{
	PhysicsHandleComp->ReleaseComponent();
	GrabObject=nullptr;
	//Particle
	if ( GunParticleComp->Template )
	{
		GunParticleComp->SetActive(false);
	}
	// UI
	if ( CrosshairUI )
	{
		CrosshairUI->SetVisibility(ESlateVisibility::Visible);
	}
	// Sound
	if ( IsLocallyControlled() )
	{
		if ( GunSoundComp )
		{
			GunSoundComp->Stop();
		}
		UGameplayStatics::PlaySound2D(GetWorld(), SFX_Drop, 1.5f);
	}
}

bool APlayerCharacter::IsOverlapPortal(bool IsLeft, FVector TargetCenter)
{
	const APortal* OtherPortal=IsLeft ? PortalRight : PortalLeft;
	if( false == OtherPortal->GetIsActivated())
	{
		return false;
	} 

	const FVector TargetBox=OtherPortal->GetComponentsBoundingBox().GetExtent();
	const float TargetMaxX=TargetCenter.X + TargetBox.X; 
	const float TargetMinX=TargetCenter.X - TargetBox.X;
	const float TargetMaxY=TargetCenter.Y + TargetBox.Y;
	const float TargetMinY=TargetCenter.Y - TargetBox.Y;
	const float TargetMaxZ=TargetCenter.Z + TargetBox.Z;
	const float TargetMinZ=TargetCenter.Z - TargetBox.Z;
	
	const FVector LinkedCenter=OtherPortal->GetActorLocation();
	const FVector LinkedBox=OtherPortal->GetComponentsBoundingBox().GetExtent();
	const float LinkedMaxX=LinkedCenter.X + LinkedBox.X;
	const float LinkedMinX=LinkedCenter.X - LinkedBox.X;
	const float LinkedMaxY=LinkedCenter.Y + LinkedBox.Y;
	const float LinkedMinY=LinkedCenter.Y - LinkedBox.Y;
	const float LinkedMaxZ=LinkedCenter.Z + LinkedBox.Z;
	const float LinkedMinZ=LinkedCenter.Z - LinkedBox.Z;
	
	const bool XOverlap = (TargetCenter.X <= LinkedCenter.X && TargetMaxX >= LinkedMinX) || (TargetCenter.X >= LinkedCenter.X && TargetMinX <= LinkedMaxX);
	const bool YOverlap = (TargetCenter.Y <= LinkedCenter.Y && TargetMaxY >= LinkedMinY) || (TargetCenter.Y >= LinkedCenter.Y && TargetMinY <= LinkedMaxY);
	const bool ZOverlap = (TargetCenter.Z <= LinkedCenter.Z && TargetMaxZ >= LinkedMinZ) || (TargetCenter.Z >= LinkedCenter.Z && TargetMinZ <= LinkedMaxZ);
	
	return XOverlap && YOverlap && ZOverlap;
}

void APlayerCharacter::ResetAllPortals()
{
	PortalLeft->Activate(false);
	PortalRight->Activate(false);

	// UI
	if(CrosshairUI)
	{
		CrosshairUI->PortalUI_Fill(true);
		CrosshairUI->PortalUI_Fill(false);
	}
}

void APlayerCharacter::ChangeVelocity(const FVector& NewDirection)
{
	const FVector CurVelocity=GetCharacterMovement()->Velocity; 
	const FVector NewVelocity=CurVelocity.Size() * NewDirection;
	GetCharacterMovement()->Velocity=NewVelocity;

	// Sound
	if(IsLocallyControlled())
	{
		if ( NewVelocity.Size() > 1000 )
		{
			UGameplayStatics::PlaySound2D(GetWorld(), SFX_PortalOutStrong, 2.0f);
		}
		else
		{
			UGameplayStatics::PlaySound2D(GetWorld(), SFX_PortalOutWeak, 1.5f);
		}
	}
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

void APlayerCharacter::PortalGunLightOn(bool IsLeft)
{
	LightComp->SetLightColor(IsLeft ? LColor : RColor);
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

void APlayerCharacter::PortalOut(const FVector& NewLocation, const FRotator& NewRotation, const FVector& NewDirection)
{
	if( HasAuthority() )
	{
		RPC_Multi_PortalOut(NewLocation, NewRotation, NewDirection);
	}
	else
	{
		RPC_Server_PortalOut(NewLocation, NewRotation, NewDirection);
	}
}

void APlayerCharacter::RPC_Server_PortalOut_Implementation(const FVector& NewLocation, const FRotator& NewRotation, const FVector& NewDirection)
{
	RPC_Multi_PortalOut(NewLocation, NewRotation, NewDirection);
}

void APlayerCharacter::RPC_Multi_PortalOut_Implementation(const FVector& NewLocation, const FRotator& NewRotation, const FVector& NewDirection)
{
	SetActorLocation(NewLocation);
	if(GetController()) GetController()->SetControlRotation(NewRotation);
	ChangeVelocity(NewDirection);
}

FVector APlayerCharacter::GetGrabPoint() const
{
	AActor* Cube=Cast<AActor>(GrabObject);
	const float CubeRadius=Cube->GetComponentsBoundingBox().GetExtent().X;
	const FVector StartVec = CameraComp->GetComponentLocation();
	const FVector EndVec = StartVec + GrabPointOffset + CameraComp->GetForwardVector() * (GrabDistance + CubeRadius);

	// 경로에 포탈이 있다면 해당 포탈의 타겟 로케이션을 반환
	FHitResult HitResult;
	FCollisionQueryParams Param;
	Param.AddIgnoredActor(this);
	Param.AddIgnoredActor(Cube);
	bool IsHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartVec, EndVec, ECC_Camera, Param);
	if( IsHit )
	{
		if( APortal* Portal = Cast<APortal>(HitResult.GetActor()) )
		{
			if( Portal->GetIsLinked() )
			{
				return Portal->GetLinkedPortal()->GetTargetLocation();
			}
		}
	}
	// 아니라면 카메라 앞 일정 거리를 반환
	return StartVec + GrabPointOffset + CameraComp->GetForwardVector() * GrabDistance;;
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
	/*GunComp->SetVisibility(true);
	GunActor->SetActive(false);*/

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
	/*GunComp->SetVisibility(false);
	GunActor->SetActive(true);
	GunActor->SetActorLocation(GunComp->GetComponentLocation());
	GunActor->SetActorRotation(GunComp->GetComponentRotation());*/

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

void APlayerCharacter::CrosshairFill(bool IsLeft)
{
	if(CrosshairUI)
	{
		CrosshairUI->PortalUI_Fill(IsLeft);
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
