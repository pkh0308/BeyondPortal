// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Portal/Portal.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PKH/Player/PlayerCharacter.h"

// Sets default values
APortal::APortal()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates=true;
	NetUpdateFrequency=60.0f;

	// Component
	BoxComp =CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(BoxComp);
	BoxComp->SetBoxExtent(FVector(5, 60, 100));
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &APortal::OnBeginOverlap);

	MeshComp=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneRef(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	if( PlaneRef.Object )
	{
		MeshComp->SetStaticMesh(PlaneRef.Object);
		MeshComp->AddRelativeRotation(FRotator(0, -90, 90));
		MeshComp->SetRelativeScale3D(FVector(1.75f, 2.0f, 1.0f));
	}

	CaptureComp=CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("CaptureComp"));
	CaptureComp->SetupAttachment(RootComponent);
	CaptureComp->AddRelativeLocation(FVector(20, 0, 0));
	CaptureComp->ShowFlags.SetDynamicShadows(false);

	ArrowComp=CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComp"));
	ArrowComp->SetupAttachment(RootComponent);
	ArrowComp->SetRelativeLocation(FVector(120, 0, 0));

	PortalSize=MeshComp->GetRelativeScale3D();
}

void APortal::BeginPlay()
{
	Super::BeginPlay();
}

void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCaptureCamera();

	if( IsCreating )
	{
		if( HasAuthority() )
		{
			DeltaTime*=PortalCreateSpeed;
			SizeAlpha=SizeAlpha + DeltaTime > 1 ? 1 : SizeAlpha + DeltaTime;
			Net_PortalScale = FMath::Lerp<FVector>(FVector::UpVector, PortalSize, SizeAlpha);
			MeshComp->SetRelativeScale3D(Net_PortalScale);

			if ( SizeAlpha >= 1 )
			{
				SizeAlpha=0;
				IsCreating=false;
			}
		}
	}
}

void APortal::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if( false == IsLinked )
	{
		return;
	}

	APortal* Portal=Cast<APortal>(OtherActor);
	if( Portal )
	{

		return;
	}

	// Location
	OtherActor->SetActorLocation(LinkedPortal->GetTargetLocation());

	// Player
	APlayerCharacter* Character=Cast<APlayerCharacter>(OtherActor);
	if( Character )
	{
		// Rotation
		const FRotator TargetRotation=LinkedPortal->GetTargetRotation();
		Character->GetController()->SetControlRotation(TargetRotation);

		// Velocity
		if( HasAuthority() )
		{
			APlayerCharacter* OwnPlayer=CastChecked<APlayerCharacter>(Player);
			OwnPlayer->ChangeVelocity(LinkedPortal->GetActorForwardVector(), AccelMultiplier);
		}
		else
		{
			RPC_PortalOut(LinkedPortal->GetActorForwardVector()); UE_LOG(LogTemp, Warning, TEXT("[RPC_PortalOut] "));
		}
		return;
	}

	// Other Actors
	UBoxComponent* Box=Cast<UBoxComponent>(OtherActor->GetRootComponent());
	if ( nullptr ==  Box )
	{
		return;
	}
	if ( Box->IsSimulatingPhysics() )
	{
		const float NewVelocity=OtherActor->GetVelocity().Size() * AccelMultiplier;
		const FVector ForwardVec=LinkedPortal->GetTargetDirection();
		Box->AddForce(ForwardVec * NewVelocity * 10000);
	}
	//OtherActor->SetActorRotation(TargetRotation);
}

void APortal::RPC_PortalOut_Implementation(const FVector& NewDirection)
{
	// Velocity
	ACharacter* OtherCharacter=UGameplayStatics::GetPlayerCharacter(GetWorld(), 1);
	if ( OtherCharacter )
	{
		UE_LOG(LogTemp, Warning, TEXT("[RPC_PortalOut] There is no idx 1 player"));
		return;
	}
	APlayerCharacter* OtherPlayer=CastChecked<APlayerCharacter>(OtherCharacter);
	OtherPlayer->ChangeVelocity(NewDirection, AccelMultiplier);
}

void APortal::Activate(const bool ActiveSelf)
{
	if(HasAuthority() ) // Server
	{
		IsActivated=ActiveSelf;
		IsCreating=ActiveSelf;

		// Material
		MeshComp->SetMaterial(0, DefaultMaterial);

		if(ActiveSelf)
		{
			BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			MeshComp->SetVisibility(true);
		}
		else
		{
			BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			MeshComp->SetVisibility(false);
		}

		// Network
		Net_PortalTransform=GetActorTransform();
	}
	else // Client
	{
		RPC_PortalTransformChanged();
	}
}

void APortal::LinkPortal(APortal* NewLinkedPortal)
{
	if( nullptr == NewLinkedPortal )
	{
		return;
	}

	// Link
	LinkedPortal=NewLinkedPortal;
	IsLinked=true;
	// Material
	MeshComp->SetMaterial(0, LinkedMaterial);
}

void APortal::UnlinkPortal()
{
	IsLinked=false;
	LinkedPortal=nullptr;
	// Material
	MeshComp->SetMaterial(0, DefaultMaterial);
}

FVector APortal::GetTargetLocation() const
{
	FVector v=ArrowComp->GetComponentLocation();
	return ArrowComp->GetComponentLocation(); 
}

FRotator APortal::GetTargetRotation() const
{
	const FVector TargetDirection=ArrowComp->GetForwardVector();
	FRotator TargetRotation=TargetDirection.ToOrientationRotator();
	TargetRotation.Roll=0;
	return TargetRotation;
}

FVector APortal::GetTargetDirection() const
{
	return ArrowComp->GetForwardVector();
}

void APortal::UpdateCaptureCamera()
{
	if(false == IsLinked )
	{
		return;
	}

	SetCaptureFOV();
	SetCaptureRotation();
}

void APortal::SetCaptureFOV()
{
	const float Distance=FVector::Dist(Player->GetActorLocation(), LinkedPortal->GetActorLocation());
	float FOVValue=FMath::Atan(Distance / FOVDivider);
	if ( FOVValue < 0.85f )
	{
		FOVValue=FMath::Lerp<float>(FOVValue, 0.85f, 0.9f);
	}
	CaptureComp->FOVAngle=FOVValue * FOVOffset;
}

void APortal::SetCaptureRotation()
{
	FVector TargetLocation=Player->GetActorLocation();
	TargetLocation.Z=0;
	FVector MyLocation=GetActorLocation();
	MyLocation.Z=0;
	FVector rightDelta = LinkedPortal->GetActorRightVector() - GetActorRightVector();
	FVector upDelta = LinkedPortal->GetActorUpVector() - GetActorUpVector();

	const FVector Direction=TargetLocation - MyLocation;
	FRotator Rotation=Direction.ToOrientationRotator();
	LinkedPortal->SetCaptureRotation(Rotation.GetNormalized());

	//FVector TargetLocation=Player->GetActorLocation();
	//TargetLocation.Z=0;
	//FVector MyLocation=GetActorLocation();
	//MyLocation.Z=0;
	//const float PitchOffset=LinkedPortal->GetActorRotation().Pitch - GetActorRotation().Pitch;
	//const float YawOffset=LinkedPortal->GetActorRotation().Yaw - GetActorRotation().Yaw;

	//const FVector Direction=TargetLocation - MyLocation;
	//FRotator Rotation=Direction.ToOrientationRotator();
	//Rotation.Pitch=0;
	//Rotation.Yaw+=YawOffset;
	//Rotation.Roll=0;
	//LinkedPortal->SetCaptureRotation(Rotation.GetNormalized());
}

void APortal::SetCaptureRotation(FRotator NewRotation)
{
	CaptureComp->SetWorldRotation(NewRotation);
}

void APortal::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APortal, Net_PortalTransform);
}

void APortal::OnRep_PortalTransformChanged()
{
	SetActorTransform(Net_PortalTransform);
}

void APortal::OnRep_PortalScaleChanged()
{
	MeshComp->SetRelativeScale3D(Net_PortalScale);
}

void APortal::RPC_PortalTransformChanged_Implementation()
{
	Net_PortalTransform=GetActorTransform();
	OnRep_PortalTransformChanged();
}
