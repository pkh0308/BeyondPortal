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
	NetUpdateFrequency = 100.0f;

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

	// Texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> SuccessTextureRef(TEXT("/Script/Engine.Texture2D'/Game/PKH/Mesh/Cube/box_03_-_Success_Emissive.box_03_-_Success_Emissive'"));
	if( SuccessTextureRef.Object )
	{
		SuccessTexture=SuccessTextureRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<UTexture2D> DefaultTextureRef(TEXT("/Script/Engine.Texture2D'/Game/PKH/Mesh/Cube/box_03_-_Default_Emissive.box_03_-_Default_Emissive'"));
	if ( DefaultTextureRef.Object )
	{
		DefaultTexture=DefaultTextureRef.Object;
	}
}

// Called when the game starts or when spawned
void AGrabCube::BeginPlay()
{
	Super::BeginPlay();

	InitDynamicMaterials();
}

void AGrabCube::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(IsDissolving)
	{
		TickDisappear(DeltaSeconds);
	}
}

// Region_Grab
void AGrabCube::Grab(ACharacter* NewOwner)
{
	/*APlayerCharacter* Player=Cast<APlayerCharacter>(NewOwner);
	if ( nullptr == Player )
	{
		return;
	}
	SetOwner(Player);

	if( HasAuthority() )
	{
		RPC_Multi_Grab(Player); UE_LOG(LogTemp, Warning, TEXT("[Server] Grab"));
	}
	else
	{
		RPC_Server_Grab(Player); UE_LOG(LogTemp, Warning, TEXT("[Client] Grab"));
	}*/

	APlayerCharacter* Player=Cast<APlayerCharacter>(NewOwner);
	if ( nullptr == Player )
	{
		return;
	}
	SetOwner(Player);

	BoxComp->SetEnableGravity(false);
}

void AGrabCube::RPC_Server_Grab_Implementation(APlayerCharacter* NewOwnPlayer)
{
	RPC_Multi_Grab(NewOwnPlayer);
}

void AGrabCube::RPC_Multi_Grab_Implementation(APlayerCharacter* NewOwnPlayer)
{
	/*if ( nullptr != OwnPlayer && nullptr != OwnPlayer->GetGrabObject() )
	{
		OwnPlayer->DropObj();
	}
	OwnPlayer=NewOwnPlayer;*/
	BoxComp->SetEnableGravity(false);

	/*if ( HasAuthority() )
	{
		OwnPlayer->GrabObj(this, BoxComp);
	}*/
}

// Region_Drop
void AGrabCube::Drop()
{
	/*if ( HasAuthority() )
	{
		RPC_Multi_Drop();
	}
	else
	{
		RPC_Server_Drop();
	}*/
	BoxComp->SetEnableGravity(true);
}

void AGrabCube::RPC_Server_Drop_Implementation()
{
	RPC_Multi_Drop();
}

void AGrabCube::RPC_Multi_Drop_Implementation()
{
	if(OwnPlayer)
	{
		OwnPlayer->DropObj();
	}
	OwnPlayer=nullptr;
	BoxComp->SetEnableGravity(true);
}

void AGrabCube::VelocityCheck()
{
	if ( HasAuthority() )
	{
		// Check Velocity
		// if velocity is too big, drop cube
		/*const FVector CurVelocity=BoxComp->GetComponentVelocity();
		if ( CurVelocity.Size() > DropVelocity )
		{
			BoxComp->SetPhysicsLinearVelocity(CurVelocity * VelocityCut);
			Drop();
		}
		else
		{
			BoxComp->SetWorldRotation(FRotator::ZeroRotator);
		}*/

		//Net_CubeLocation=GetActorLocation();
		//Net_CubeRotation=GetActorRotation();
	}
}

void AGrabCube::ChangeMaterial(bool Success)
{
	if(Success)
	{
		DMArray[2]->SetTextureParameterValue(FName(TEXT("Emissive")), SuccessTexture);
	}
	else
	{
		DMArray[2]->SetTextureParameterValue(FName(TEXT("Emissive")), DefaultTexture);
	}
}

void AGrabCube::InitDynamicMaterials()
{
	const int32 Nums=MeshComp->GetNumMaterials();
	for ( int i = 0; i < Nums; i++ )
	{
		DMArray.Add(UMaterialInstanceDynamic::Create(MeshComp->GetMaterial(i), this));
		MeshComp->SetMaterial(i, DMArray[i]);
	}
}

void AGrabCube::OnDisappear()
{
	IsDissolving=true; 
}

void AGrabCube::TickDisappear(float DeltaSeconds)
{
	DissolveCount=(DissolveCount + DeltaSeconds / DissolveSeconds);
	for ( int i=0; i < DMArray.Num(); i++ )
	{
		DMArray[i]->SetScalarParameterValue(FName(TEXT("Fade")), DissolveCount); 
	}

	if( DissolveCount > 1.0f )
	{
		IsDissolving=false;
		Drop();
		Destroy();
	}
}

//void AGrabCube::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	DOREPLIFETIME(AGrabCube, Net_CubeLocation);
//	DOREPLIFETIME(AGrabCube, Net_CubeRotation);
//}
//
//void AGrabCube::OnRep_CubeLocationChanged()
//{
//	SetActorLocation(Net_CubeLocation);
//}
//
//void AGrabCube::OnRep_CubeRotationChanged()
//{
//	SetActorRotation(Net_CubeRotation);
//}
