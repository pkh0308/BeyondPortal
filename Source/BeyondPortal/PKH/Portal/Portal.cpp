// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Portal/Portal.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneCaptureComponent2D.h"
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
	BoxComp->SetBoxExtent(FVector(25, 60, 100));
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
		UE_LOG(LogTemp, Log, TEXT("this->IsNew: %d"), IsNew);
		UE_LOG(LogTemp, Log, TEXT("other->IsNew: %d"), Portal->IsNew);
		if(Portal->LinkedPortal == this )
		{
			return;
		}
		
		Activate(false);
		return;
	}
	// Check Delay
	if ( IsInDelay )
	{
		return;
	}
	IsInDelay=true;

	GetWorldTimerManager().SetTimer(DelayHandle, FTimerDelegate::CreateLambda([this]()
	{
		IsInDelay=false;
	}), 0.1f, false);

	// Location & Rotation
	const FVector TargetLocation =  LinkedPortal->GetTargetLocation();
	const FRotator TargetRotation = LinkedPortal->GetTargetRotation();
	const FVector TargetDirection = GetLinkedPortal()->GetActorForwardVector();

	// Player
	APlayerCharacter* Character=Cast<APlayerCharacter>(OtherActor);
	if( Character )
	{
		if ( Character->HasAuthority() )
		{
			Character->PortalOut(TargetLocation, TargetRotation, TargetDirection);
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
		// Location
		OtherActor->SetActorLocation(TargetLocation);

		// Velocity
		const float NewVelocity=OtherActor->GetVelocity().Size();
		const FVector ForwardVec=LinkedPortal->GetTargetDirection();
		Box->AddForce(ForwardVec * NewVelocity * 10000);
	}
	//OtherActor->SetActorRotation(TargetRotation);
}

void APortal::Activate(const bool ActiveSelf)
{
	if(HasAuthority() ) // Server
	{
		IsActivated=ActiveSelf;
		IsCreating=ActiveSelf;
		IsNew=false;

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

			if(IsLinked && LinkedPortal != nullptr)
			{
				LinkedPortal->UnlinkPortal();
				this->UnlinkPortal();
			}
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
	if( HasAuthority() )
	{
		RPC_Multi_MaterialChange(LinkedMaterial);
	}
	else
	{
		RPC_Server_MaterialChange(LinkedMaterial);
	}
}

void APortal::RPC_Server_MaterialChange_Implementation(class UMaterialInterface* NewMaterial)
{
	RPC_Multi_MaterialChange(LinkedMaterial);
}

void APortal::RPC_Multi_MaterialChange_Implementation(class UMaterialInterface* NewMaterial)
{
	MeshComp->SetMaterial(0, NewMaterial);
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
	const auto PlayerCameraManager=GWorld->GetFirstPlayerController()->PlayerCameraManager;

	FMatrix ViewMatrix;
	FMatrix ProjectionMatrix;
	FMatrix ViewProjectionMatrix;
	UGameplayStatics::GetViewProjectionMatrix(PlayerCameraManager->GetCameraCacheView(), ViewMatrix, ProjectionMatrix, ViewProjectionMatrix);

	CaptureComp->CustomProjectionMatrix=ProjectionMatrix;

	//const FVector TargetLocation=Player->GetActorLocation();
	////TargetLocation.Z=0;
	//const FVector MyLocation=GetActorLocation();
	////MyLocation.Z=0;
	//const FRotator RotationOffset = LinkedPortal->GetActorRotation() - GetActorRotation();

	//if(FMath::Abs(GetActorForwardVector().Z) > 0.9f )
	//{
	//	// Horizontal Wall
	//	const auto PlayerCameraManager=GWorld->GetFirstPlayerController()->PlayerCameraManager;

	//	FMatrix ViewMatrix;
	//	FMatrix ProjectionMatrix;
	//	FMatrix ViewProjectionMatrix;
	//	UGameplayStatics::GetViewProjectionMatrix(PlayerCameraManager->GetCameraCacheView(), ViewMatrix, ProjectionMatrix, ViewProjectionMatrix);

	//	LinkedPortal->SetCaptureRotation(ProjectionMatrix); 
	//}
	//else 
	//{
	//	// Vertical Wall
	//	const FVector Direction=(TargetLocation - LinkedPortal->GetActorLocation()).GetSafeNormal();
	//	FRotator Rotation=Direction.ToOrientationRotator();
	//	Rotation.Yaw+=RotationOffset.Yaw;
	//	SetCaptureRotation(Rotation);
	//}
}

void APortal::SetCaptureRotation(FRotator NewRotation)
{
	CaptureComp->SetWorldRotation(NewRotation);
}

void APortal::SetCaptureRotation(FMatrix NewMatrix)
{
	CaptureComp->CustomProjectionMatrix=NewMatrix;
}

void APortal::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APortal, Net_PortalTransform);
	DOREPLIFETIME(APortal, Net_PortalScale);
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
