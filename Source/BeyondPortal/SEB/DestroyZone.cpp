// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/DestroyZone.h"

#include "ArmDoor.h"
#include "PortalButton.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PKH/Player/PlayerCharacter.h"
#include "PKH/Props/GrabCube.h"

// Sets default values
ADestroyZone::ADestroyZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	boxComp=CreateDefaultSubobject<UBoxComponent>(TEXT("boxComp"));
	boxComp->SetBoxExtent(FVector(50));
	this->SetRootComponent(boxComp);
	destroyZone=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("destroyZone"));
	destroyZone->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ADestroyZone::BeginPlay()
{
	Super::BeginPlay();
	boxComp->OnComponentBeginOverlap.AddDynamic(this, &ADestroyZone::OnMyCompBeginOverlap);
	boxComp->OnComponentEndOverlap.AddDynamic(this, &ADestroyZone::OnMyCompEndOverlap);
}

// Called every frame
void ADestroyZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADestroyZone::OnMyCompBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// DestroyZone이랑 Cube랑 닿았을 때 큐브 제거 
	if ( OtherActor->IsA<AGrabCube>() ) {

		TArray<AActor*> FoundActors;
		FString findTag=OtherActor->Tags.Num() > 0 ? OtherActor->Tags[0].ToString() : TEXT("NoTag");
		FName findTagName=FName(*findTag);

		if ( findTag != "NoTag" )
		{
			UGameplayStatics::GetAllActorsWithTag(GetWorld(), findTagName, FoundActors);
			if ( FoundActors[0] )
			{
				APortalButton* pb=Cast<APortalButton>(FoundActors[0]);
				if ( pb )pb->isSpawned=false;
				else GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("PB가 없다"));
			}
		}

		//cube 사라지는 이펙트 후 큐브 제거
		AGrabCube* cube=Cast<AGrabCube>(OtherActor);
		cube->OnDisappear();
		
	}

	
	else if ( OtherActor->IsA< APlayerCharacter>() ) {
		
		FString findMyTag=this->Tags.Num() > 0 ? this->Tags[0].ToString() : TEXT("NoTag");

		if( findMyTag == "opendoor" )
		{
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AArmDoor::StaticClass(), FoundDoors);
			float Delay=0.1f;
			cnt1++;
			for ( auto findDoor : FoundDoors )
			{
				// 딜레이 후에 문 열기
				AArmDoor* Door=Cast<AArmDoor>(findDoor);
				if ( cnt1 >= 2 )
					Door->openDoor(findDoor, Delay);
				Delay+=0.1f;
			}
		}
		else if( findMyTag == "closedoor" && !isCheckClosed ) 
		{
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AArmDoor::StaticClass(), FoundDoors);
			float Delay=0.1f;
			cnt2++;
			UE_LOG(LogTemp, Error, TEXT("cnt : %d"), cnt2);
			//float Delay=0.1f;
			for ( auto findDoor : FoundDoors )
			{
				// 딜레이 후에 문 열기
				AArmDoor* Door=Cast<AArmDoor>(findDoor);
				if(cnt2>=2 )
				{
					Door->closeDoor(findDoor, Delay);
					isCheckClosed=true;
				}
				Delay+=0.1f;
			}

			
		}
		else if(findMyTag == "NoTag" )
		{
			APlayerCharacter* player=Cast<APlayerCharacter>(OtherActor);
			player->OnDie();
			player->ResetAllPortals();
		}
		
	}
}

void ADestroyZone::OnMyCompEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	FString findMyTag=this->Tags.Num() > 0 ? this->Tags[0].ToString() : TEXT("NoTag");

	if ( OtherActor->IsA< APlayerCharacter>() )
	{
		if ( findMyTag == "opendoor" )
		{
			
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AArmDoor::StaticClass(), FoundDoors);
			float Delay=0.1f;
			cnt1--;
		}
		else if( findMyTag == "closedoor" && !isCheckClosed ) 
		{
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AArmDoor::StaticClass(), FoundDoors);
			cnt2--;

			
		}
		else if(findMyTag == "NoTag" )
		{
			
		}
	}
}


void ADestroyZone::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADestroyZone, cnt1);
	DOREPLIFETIME(ADestroyZone, cnt2);
	DOREPLIFETIME(ADestroyZone, isCheckClosed);
	
}

