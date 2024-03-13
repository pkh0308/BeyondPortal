// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/Door.h"

#include "PortalButton.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PKH/Player/PlayerCharacter.h"

// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	boxComp=CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(boxComp);

	// mesh 생성
	door=CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("door"));
	// 루트로 설정
	door->SetupAttachment(RootComponent);
	//this->SetRootComponent(RootComponent);

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/SEB/Resources/Mesh/PortalDoor/PortalDoor.PortalDoor'"));
	if ( tempMesh.Succeeded() )
	{
		door->SetSkeletalMesh(tempMesh.Object);
		door->SetRelativeScale3D(FVector(0.004f));
	}
	bReplicates=true;
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	boxComp->OnComponentBeginOverlap.AddDynamic(this, &ADoor::OnMyCompBeginOverlap);
	boxComp->OnComponentEndOverlap.AddDynamic(this, &ADoor::OnMyCompEndOverlap);

}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADoor::OnMyCompBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	// Player가 앞에 와서 충돌하면
	if ( OtherActor->IsA<APlayerCharacter>() ) {
		
		cnt++;
		UE_LOG(LogTemp, Error, TEXT("cnt : %d"), cnt);
		OpenDoor();
	}
}

void ADoor::OnMyCompEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Player가 앞에 와서 충돌하면
	if ( OtherActor->IsA<APlayerCharacter>() ) {

		cnt--;
	}
}


void ADoor::OpenDoor()
{
	if ( cnt >= 2 && !isOpened)
	{
		RPC_Server_OpenDoor();
	}
	else
	{
		
	}

}


void ADoor::RPC_Server_OpenDoor_Implementation()
{
	RPC_Multi_OpenDoor();
}

void ADoor::RPC_Multi_OpenDoor_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("열려라"));
	door->PlayAnimation(openDoorAnim, false);
	isOpened=true;
}

void ADoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADoor, openDoorAnim);
	DOREPLIFETIME(ADoor, cnt);
}
