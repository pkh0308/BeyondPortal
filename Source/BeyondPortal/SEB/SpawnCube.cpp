// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/SpawnCube.h"

#include "BarrierButton.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASpawnCube::ASpawnCube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	static ConstructorHelpers::FObjectFinder<USoundBase> SFX_SpawnCubeRef(TEXT("/Script/Engine.SoundWave'/Game/SEB/Resources/Sounds/spawncube.spawncube'"));
	if ( SFX_SpawnCubeRef.Object )
	{
		SFX_SpawnCube=SFX_SpawnCubeRef.Object;
	}
}

// Called when the game starts or when spawned
void ASpawnCube::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABarrierButton::StaticClass(), BarrierButtonCheck);
	
}

// Called every frame
void ASpawnCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ABarrierButton* bb=Cast<ABarrierButton>(BarrierButtonCheck[0]);
	ABarrierButton* bb2=Cast<ABarrierButton>(BarrierButtonCheck[1]);

	if(!isSpawned )
	{
		if(bb->isCheck && bb2->isCheck )
		{
			UGameplayStatics::PlaySound2D(GetWorld(),SFX_SpawnCube, 0.3f );

			RPC_Server_Spawn();
			isSpawned=true;
		}
	}
}

void ASpawnCube::Spawn()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("CubeDropper"), FoundActors);

	float NearestActorDistance=1000.0f;
	AActor* NearestActor=UGameplayStatics::FindNearestActor(GetActorLocation(), FoundActors, NearestActorDistance);

	//스폰된 Cube가 없다면 cube 스폰
	AActor* cube=GetWorld()->SpawnActor<AActor>(spawnCube, FVector(NearestActor->GetActorLocation().X, NearestActor->GetActorLocation().Y, NearestActor->GetActorLocation().Z - 200), FRotator::ZeroRotator);

}

void ASpawnCube::RPC_Server_Spawn_Implementation()
{
	Spawn();
}







