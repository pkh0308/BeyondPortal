// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/FloorLine.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AFloorLine::AFloorLine()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick=true;
	spline=CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	instancedStaticMeshComp=CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedStaticMesh"));
}

void AFloorLine::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if ( chain && spline && instancedStaticMeshComp )
	{
		UE_LOG(LogTemp, Error, TEXT("있네유"));
		instancedStaticMeshComp->SetStaticMesh(chain);
		FVector breakBoxChain=chain->GetBoundingBox().Max - chain->GetBoundingBox().Min;
		float spacing=breakBoxChain.X + offset;

		int32 a=floor(spline->GetSplineLength() / spacing);

		for ( int i=0; i < a; i++ )
		{

			FVector locationSpline=spline->GetLocationAtDistanceAlongSpline(i * spacing, ESplineCoordinateSpace::Local);
			FTransform transformSpline=UKismetMathLibrary::MakeTransform(locationSpline, FRotator(0, 0, 0), FVector3d(1, 1, 1));
			instancedStaticMeshComp->AddInstance(transformSpline);

		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("읎네요"));
	}




}

// Called when the game starts or when spawned
void AFloorLine::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AFloorLine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

