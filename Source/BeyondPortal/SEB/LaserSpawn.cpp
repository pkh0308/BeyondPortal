// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/LaserSpawn.h"

#include "Laser.h"
#include "Kismet/KismetSystemLibrary.h"

ALaserSpawn::ALaserSpawn()
{
	arrow=CreateDefaultSubobject<UArrowComponent>(TEXT("arrow"));
	arrow->SetupAttachment(RootComponent);

	arrow->SetRelativeLocation(FVector(0, 0, 50));
	arrow->SetRelativeRotation(FRotator(90, 0, 0));
}

void ALaserSpawn::BeginPlay()
{
	Super::BeginPlay();
	//SetSpawnReference(this);
	SpawnReference=this;
	ID=UKismetSystemLibrary::GetDisplayName(this);

	//ALaser* laser=Cast<ALaser>(GetWorld()->SpawnActor<AActor>(ALaser::StaticClass(), arrow->GetComponentLocation(), this->GetActorRotation()));

	ALaser* laser=Cast<ALaser>(GetWorld()->SpawnActor<AActor>(laserFactory, arrow->GetComponentLocation(), this->GetActorRotation()));
	laser->Laser_ID=ID;
	laser->LaserColor = LaserColor;
	laser->SpawnReference = SpawnReference;
	ChildLasers.Add(laser);
}

void ALaserSpawn::SetSpawnReference(ALaserSpawn* NewSpawnReference)
{
	SpawnReference=NewSpawnReference;
}
