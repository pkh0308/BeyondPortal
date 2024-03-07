// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/CheckOpen.h"

// Sets default values
ACheckOpen::ACheckOpen()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	checkOpen=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("checkOpen"));
	checkOpen->SetRelativeScale3D(FVector(1, 0.2, 1));

	

}

// Called when the game starts or when spawned
void ACheckOpen::BeginPlay()
{
	Super::BeginPlay();
    
}

// Called every frame
void ACheckOpen::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

