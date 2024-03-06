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

	// For Server
	if ( false == HasAuthority() )
	{
		return;
	}
	// Set Network Owner
	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]()
	{
		for ( FConstPlayerControllerIterator it=GetWorld()->GetPlayerControllerIterator(); it; ++it )
		{
			APlayerController* PlayerController=it->Get();
			// WeakPtr이므로 널체크
			// LocalPlayerController가 아니라면(서버의 컨트롤러가 아니라면) 클라이언트
			if ( nullptr != PlayerController && false == PlayerController->IsLocalPlayerController() )
			{
				SetOwner(PlayerController); UE_LOG(LogTemp, Warning, TEXT("SetOwner"));
				break;
			}
		}
	}), 5.0f, false);
}

void AGrabCube::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if( OwnPlayer )
	{
		TickGrab();
	}
	if(IsDissolving)
	{
		TickDisappear(DeltaSeconds);
	}
}

void AGrabCube::Grab(ACharacter* NewOwner)
{
	APlayerCharacter* Player=Cast<APlayerCharacter>(NewOwner);
	if ( nullptr == Player )
	{
		return;
	}

	if( HasAuthority() )
	{
		if ( nullptr != OwnPlayer && nullptr != OwnPlayer->GetGrabObject() )
		{
			OwnPlayer->DropObj();
		}
		OwnPlayer=Player;
		OwnPlayer->GrabObj(this);
		BoxComp->SetEnableGravity(false);
		// Network
		Net_OwnPlayer=Player;
	}
	else
	{
		RPC_SetCubeOwner(Player);
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
		RPC_SetCubeOwner(nullptr);
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
	else
	{
		
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
	if( Net_OwnPlayer )
	{
		if(nullptr != OwnPlayer && nullptr != OwnPlayer->GetGrabObject())
		{
			OwnPlayer->DropObj();
		}
		OwnPlayer=Net_OwnPlayer;

		OwnPlayer->GrabObj(this);
		BoxComp->SetEnableGravity(false);
	}
	else
	{
		if ( OwnPlayer )
		{
			OwnPlayer->DropObj();
		}
		OwnPlayer=Net_OwnPlayer;
		BoxComp->SetEnableGravity(true);
	}
}

void AGrabCube::RPC_SetCubeOwner_Implementation(APlayerCharacter* NewOwner)
{
	Net_OwnPlayer=NewOwner;
	OnRep_OwnPlayerChanged();
}
