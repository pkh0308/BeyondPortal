// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Portal/Portal.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "GameFramework/Character.h"


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
		MeshComp->SetRelativeScale3D(FVector(1.5f, 1.75, 1.0f));
	}

	CaptureComp=CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("CaptureComp"));
	CaptureComp->SetupAttachment(RootComponent);
	CaptureComp->AddRelativeLocation(FVector(10, 0, 0));

	ArrowComp=CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComp"));
	ArrowComp->SetupAttachment(RootComponent);
	ArrowComp->SetRelativeLocation(FVector(120, 0, 0));
}

void APortal::BeginPlay()
{
	Super::BeginPlay();

	// Capture
	
}

void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if( false == IsLinked )
	{
		return;
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
	}
	else
	{
		OtherActor->SetActorRotation(TargetRotation);
	}
}

void APortal::Activate(const bool ActiveSelf)
{
	IsActivated=ActiveSelf;
	CaptureLocation=CaptureComp->GetComponentLocation();
	CaptureRotation=CaptureComp->GetComponentRotation(); 
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
	CaptureComp->SetWorldLocation(LinkedPortal->GetCaptureLocation());
	CaptureComp->SetWorldRotation(LinkedPortal->GetCaptureRotation());
	CaptureComp->Activate(true);
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

void APortal::ResetCaptureCamera()
{
	CaptureComp->SetRelativeLocation(FVector(10, 0, 0));
	CaptureComp->SetRelativeRotation(FRotator(0));
}
