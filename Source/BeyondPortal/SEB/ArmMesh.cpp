// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/ArmMesh.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AArmMesh::AArmMesh()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// mesh 생성
	armMesh=CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("armMesh"));
	// 루트에 붙임
	armMesh->SetupAttachment(RootComponent);

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/SEB/Resources/Mesh/ArmDoor/arm64x64_interior_model.arm64x64_interior_model'"));
	if ( tempMesh.Succeeded() )
	{
		armMesh->SetSkeletalMesh(tempMesh.Object);
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> SFX_OpenDoorRef(TEXT("/Script/Engine.SoundWave'/Game/SEB/Resources/Sounds/DoorOpen.DoorOpen'"));
	if ( SFX_OpenDoorRef.Object )
	{
		SFX_OpenDoor=SFX_OpenDoorRef.Object;
	}
	bReplicates=true;
}

// Called when the game starts or when spawned
void AArmMesh::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AArmMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AArmMesh::openMesh()
{
	armMesh->PlayAnimation(openMeshAnim, false);
	UGameplayStatics::PlaySound2D(GetWorld(),SFX_OpenDoor, 1.0f );

}



void AArmMesh::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AArmMesh, openMeshAnim);
}
