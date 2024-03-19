// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Props/LaserBlock.h"

// Sets default values
ALaserBlock::ALaserBlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALaserBlock::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALaserBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

