// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/LasersParent.h"

#include "Laser.h"
#include "Kismet/KismetArrayLibrary.h"

// Sets default values
ALasersParent::ALasersParent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SM_Parent=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_Parent"));
	SetRootComponent(SM_Parent);
}

// Called when the game starts or when spawned
void ALasersParent::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALasersParent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALasersParent::ClearAllLasersAttached()
{
	for (auto laser : LasersAttached)
	{
		if(laser!=nullptr)
		{
			laser->Destroy();
		}
	}

	LasersAttached.Empty();
}

