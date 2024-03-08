// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Props/GunActor.h"

#include "Components/BoxComponent.h"

// Sets default values
AGunActor::AGunActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// Box
	BoxComp=CreateDefaultSubobject<UBoxComponent>(TEXT("CapsuleComp"));
	SetRootComponent(BoxComp);
	BoxComp->SetBoxExtent(FVector(20.0f, 8.0f, 8.0f));
	BoxComp->SetSimulatePhysics(true);
	BoxComp->SetCollisionProfileName(TEXT("BlockAll"));

	// StaticMesh
	MeshComp=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCollisionProfileName(TEXT("NoCollision"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshCompRef(TEXT("/Script/Engine.StaticMesh'/Game/PKH/Mesh/PortalGun/SM_PortalGun.SM_PortalGun'"));
	if( MeshCompRef.Object )
	{
		MeshComp->SetStaticMesh(MeshCompRef.Object);
		MeshComp->SetRelativeScale3D(FVector(5.0f));
	}
}

void AGunActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGunActor::SetActive(bool Active)
{
	if(Active )
	{
		BoxComp->SetCollisionProfileName(TEXT("BlockAll"));
	}
	else
	{
		BoxComp->SetCollisionProfileName(TEXT("NoCollision"));
	}
	MeshComp->SetVisibility(Active);
}
