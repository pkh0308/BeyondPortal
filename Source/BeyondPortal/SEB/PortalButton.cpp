// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/PortalButton.h"

#include "ArmDoor.h"
#include "ArmMesh.h"
#include "Barrier.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PKH/Player/PlayerCharacter.h"
#include "PKH/Props/GrabCube.h"

// Sets default values
APortalButton::APortalButton()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick=true;

	// capsuleComp를 생성
	capsuleComp=CreateDefaultSubobject<UCapsuleComponent>(TEXT("capsuleComp"));
	// 루트 컴포넌트로 변경
	this->SetRootComponent(capsuleComp);
	capsuleComp->SetCapsuleHalfHeight(100);
	capsuleComp->SetCapsuleRadius(30);

	// mesh 생성
	portalButton=CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("portalButton"));
	// 루트에 붙임
	portalButton->SetupAttachment(RootComponent);

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/SEB/Resources/Mesh/PedestalButton/PortalButton.PortalButton'"));
	if ( tempMesh.Succeeded() )
	{
		portalButton->SetSkeletalMesh(tempMesh.Object);
		portalButton->SetRelativeLocation(FVector(0, 0, -70));
		portalButton->SetRelativeScale3D(FVector(0.02f));
	}

	bReplicates=true;
}

// Called when the game starts or when spawned
void APortalButton::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APortalButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}


void APortalButton::activeSpawnCube()
{
	ABarrier* foundActor=Cast<ABarrier>(UGameplayStatics::GetActorOfClass(GetWorld(), APortalButton::StaticClass()));
	FString findTag=this->Tags.Num() > 0 ? this->Tags[0].ToString() : TEXT("NoTag");
	FName findTagName=FName(*findTag);


	// 발판 UP
	if ( findTag == TEXT("open") )
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("열려라"));
		TArray<AActor*> findArmMesh;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AArmMesh::StaticClass(), findArmMesh);
		findArmMesh.Sort([](const AActor& a, const AActor& b)
		{
			return a.GetActorLabel() < b.GetActorLabel();
		});
		float Delay=0.1f;
		for ( auto currentArm : findArmMesh )
		{
			// 딜레이 후에 문 열기
			AArmMesh* Mesh=Cast<AArmMesh>(currentArm);
			FTimerHandle Handle;
			GetWorldTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([Mesh]()
				{
					Mesh->openMesh();
				}), Delay, false);
			Delay+=0.1f;

		}
	}
	// 큐브 스폰
	else
	{
		if (!isSpawned )
		{
			GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Green, TEXT("스폰!!"));
			//tag가 CubeDropper인 액터 탐색
			TArray<AActor*> FoundActors;
			UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("CubeDropper"), FoundActors);

			//그 중에 가장 가까운 것 탐색 후 위치를 얻어서 => Find Nearest actor
			float NearestActorDistance=1000.0f;
			AActor* NearestActor=UGameplayStatics::FindNearestActor(GetActorLocation(), FoundActors, NearestActorDistance);

			//스폰된 Cube가 없다면 cube 스폰
			AActor* cube=GetWorld()->SpawnActor<AActor>(spawnCube, FVector(NearestActor->GetActorLocation().X, NearestActor->GetActorLocation().Y, NearestActor->GetActorLocation().Z - 200), FRotator::ZeroRotator);
			

			cube->Tags.Add(findTagName);
			isSpawned=true;


		}
		else
		{
			
		}
		
	}

}


void APortalButton::SpawnCube()
{
}

void APortalButton::ServerSpawnCube_Implementation()
{
	MultiSpawnCube();
}

void APortalButton::MultiSpawnCube_Implementation()
{
	
}



//E 버튼 눌렸을 때
void APortalButton::DoInteraction()
{
	portalButton->PlayAnimation(pressButtonAnim, false);

	//GetWorld()->SpawnActor<AGrabCube>(FVector(0, 0, 0), FRotator::ZeroRotator);

	GetWorld()->SpawnActor<AActor>(spawnCube, FVector(0, 0, 1000), FRotator::ZeroRotator);
	  
}



void APortalButton::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
