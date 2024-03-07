// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/DestroyZone.h"

#include "PortalButton.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
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
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("큐브가 사라졌으니까"));


	}

	//Barrier랑 Player랑 닿았을 때 포탈 초기화
	else if ( OtherActor->IsA< APlayerCharacter>() ) {
		APlayerCharacter* player=Cast<APlayerCharacter>(OtherActor);
		player->ResetAllPortals();
	}
}

