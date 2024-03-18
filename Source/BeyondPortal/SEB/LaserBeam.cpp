// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/LaserBeam.h"

#include "Kismet/KismetMathLibrary.h"

// Sets default values
ALaserBeam::ALaserBeam()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Beam=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Beam"));
	SetRootComponent(Beam);



}

// Called when the game starts or when spawned
void ALaserBeam::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALaserBeam::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALaserBeam::SetEnds(FVector Begin, FVector End)
{
	UE_LOG(LogTemp, Error, TEXT("SetEnds"));
	FRotator newRot=UKismetMathLibrary::MakeRotFromZ(UKismetMathLibrary::Normal(End - Begin));
	Beam->SetWorldLocationAndRotation(Begin, newRot, false, nullptr, ETeleportType::TeleportPhysics);
	Beam->SetWorldScale3D(FVector(0.05, 0.05, UKismetMathLibrary::VSize((End - Begin) / 100.0f)));
}

