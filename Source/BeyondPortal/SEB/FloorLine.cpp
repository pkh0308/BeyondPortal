// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/FloorLine.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AFloorLine::AFloorLine()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick=true;
	lineMeshComp=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("lineMeshComp"));

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Game/SEB/Resources/Mesh/FloorLine_Blue.FloorLine_Blue'"));
	if ( tempMesh.Succeeded() )
	{
		lineMeshComp->SetStaticMesh(tempMesh.Object);

	}
}

void AFloorLine::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	
}


// Called when the game starts or when spawned
void AFloorLine::BeginPlay()
{
	Super::BeginPlay();

	//시작할 때는 주황색 라인은 안보이도록 함. 
	FString findColor=this->Tags.Num() > 1 ? this->Tags[1].ToString() : TEXT("NoTag");
	if( findColor == "orange" )
	{
		this->SetActorHiddenInGame(true);
	}
}

// Called every frame
void AFloorLine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}



