// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Props/GrabCube.h"
#include "Components/BoxComponent.h"
#include "PKH/Player/PlayerCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AGrabCube::AGrabCube()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	NetUpdateFrequency = 60.0f;

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
	if( HasAuthority() )
	{
		APlayerCharacter* Player=Cast<APlayerCharacter>(NewOwner);
		if ( nullptr == Player )
		{
			return;
		}
		OwnPlayer=Player;
		BoxComp->SetEnableGravity(false);
		// Network
		Net_OwnPlayer=Player;
	}
	else
	{
		
	}
}

void AGrabCube::Drop()
{
	if ( HasAuthority() )
	{
		OwnPlayer->DropObj();
		OwnPlayer=nullptr;
		BoxComp->SetEnableGravity(true);
		// Network
		Net_OwnPlayer=nullptr;
	}
	else
	{
		
	}
}

void AGrabCube::TickGrab()
{
	// Server
	if( HasAuthority() )
	{
		const FVector NewLocation=OwnPlayer->GetGrabPoint();
		SetActorLocation(NewLocation);

		// Check Velocity
		// if velocity is too big, drop cube
		const FVector CurVelocity=BoxComp->GetComponentVelocity();
		if ( CurVelocity.Size() > DropVelocity )
		{
			BoxComp->SetPhysicsLinearVelocity(CurVelocity * VelocityCut);
			Drop();
		}

		Net_CubeLocation=GetActorLocation();
		Net_CubeRotation=GetActorRotation();
	}
}

void AGrabCube::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGrabCube, Net_CubeLocation);
	DOREPLIFETIME(AGrabCube, Net_CubeRotation);
	DOREPLIFETIME(AGrabCube, Net_OwnPlayer);
}

void AGrabCube::OnRep_CubeLocationChanged()
{
	SetActorLocation(Net_CubeLocation);
}

void AGrabCube::OnRep_CubeRotationChanged()
{
	SetActorRotation(Net_CubeRotation);
}

void AGrabCube::OnRep_OwnPlayerChanged()
{
	OwnPlayer=Net_OwnPlayer;

	if( OwnPlayer )
	{
		BoxComp->SetEnableGravity(false);
	}
	else
	{
		BoxComp->SetEnableGravity(true);
	}
}
