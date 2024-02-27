// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Portal/Portal.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
APortal::APortal()
{
	PrimaryActorTick.bCanEverTick = true;

	// Component
	BoxComp =CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(BoxComp);
	BoxComp->SetBoxExtent(FVector(5, 52, 90));
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &APortal::OnBeginOverlap);

	MeshComp=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneRef(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	if( PlaneRef.Object )
	{
		MeshComp->SetStaticMesh(PlaneRef.Object);
		MeshComp->AddRelativeRotation(FRotator(0, -90, 90));
		MeshComp->SetRelativeScale3D(FVector(1.5f, 1.75f, 1.0f));
	}

	CaptureComp=CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("CaptureComp"));
	CaptureComp->SetupAttachment(RootComponent);
	CaptureComp->AddRelativeLocation(FVector(10, 0, 0));

	ArrowComp=CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComp"));
	ArrowComp->SetupAttachment(RootComponent);
	ArrowComp->SetRelativeLocation(FVector(120, 0, 0));

	PortalSize=MeshComp->GetRelativeScale3D();
}

void APortal::BeginPlay()
{
	Super::BeginPlay();

	Player=UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCaptureCamera();

	if( IsCreating )
	{
		DeltaTime*=PortalCreateSpeed;
		SizeAlpha = SizeAlpha + DeltaTime > 1 ? 1 : SizeAlpha + DeltaTime;
		MeshComp->SetRelativeScale3D(FMath::Lerp<FVector>(FVector::UpVector, PortalSize, SizeAlpha));

		if(SizeAlpha >= 1)
		{
			SizeAlpha=0;
			IsCreating=false;
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

	OtherActor->SetActorLocation(LinkedPortal->GetTargetLocation());

	const FRotator TargetRotation=LinkedPortal->GetTargetRotation();
	ACharacter* Character=Cast<ACharacter>(OtherActor);
	if( Character )
	{
		Character->GetController()->SetControlRotation(TargetRotation);

		// Velocity
		const float NewVelocity=Character->GetRootComponent()->ComponentVelocity.Size();
		const FVector ForwardVec=ArrowComp->GetForwardVector();
		Character->GetRootComponent()->ComponentVelocity = ForwardVec * NewVelocity;
	}
	else
	{
		OtherActor->SetActorRotation(TargetRotation);
	}
}

void APortal::Activate(const bool ActiveSelf)
{
	IsActivated=ActiveSelf;
	IsCreating=true;

	// Material
	MeshComp->SetMaterial(0, DefaultMaterial);
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

	const float Distance=FVector::Dist(Player->GetActorLocation(), LinkedPortal->GetActorLocation());
	const float FOVValue = FMath::Clamp(Distance / FOVDivider, 30, 150);
	CaptureComp->FOVAngle=FOVValue;
}
