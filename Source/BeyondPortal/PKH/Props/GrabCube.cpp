// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Props/GrabCube.h"
#include "Components/BoxComponent.h"
#include "PKH/Player/PlayerCharacter.h"

// Sets default values
AGrabCube::AGrabCube()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxComp=CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCom"));
	SetRootComponent(BoxComp);
	BoxComp->SetBoxExtent(FVector(50, 50, 50));
	BoxComp->SetSimulatePhysics(true);

	MeshComp=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(TEXT("/Script/Engine.StaticMesh'/Game/PKH/Mesh/Cube/Cube.Cube'"));
	if ( MeshRef.Object )
	{
		MeshComp->SetStaticMesh(MeshRef.Object);
	}

}

// Called when the game starts or when spawned
void AGrabCube::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGrabCube::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if( OwnPlayer )
	{
		TickGrab();
	}
}

void AGrabCube::Grab(ACharacter* NewOwner)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(NewOwner);
	if( Player )
	{
		OwnPlayer=Player;
	}
	BoxComp->SetSimulatePhysics(false);
}

void AGrabCube::Drop()
{
	OwnPlayer=nullptr;
	BoxComp->SetSimulatePhysics(true);
}

void AGrabCube::TickGrab()
{
	FVector NewLocation=OwnPlayer->GetGrabPoint();
	if(FVector::Dist(NewLocation, GetActorLocation()) > 250.0f )
	{
		OwnPlayer=nullptr;
	}
	else
	{
		SetActorLocation(NewLocation);
	}
}
