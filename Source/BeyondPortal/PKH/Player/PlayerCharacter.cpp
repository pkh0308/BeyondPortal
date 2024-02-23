// Fill out your copyright notice in the Description page of Project Settings.


#include "PKH/Player/PlayerCharacter.h"
#include "PlayerInputComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Setting
	

	// Component
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalRef(TEXT("/Script/Engine.SkeletalMesh'/Game/PKH/Mesh/Player/SKM_Player_Man.SKM_Player_Man'"));
	if( SkeletalRef.Object)
	{
		GetMesh()->SetSkeletalMesh(SkeletalRef.Object);
	}

	GunComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunComp"));
	GunComp->SetupAttachment(GetMesh());

	InputComp = CreateDefaultSubobject<UPlayerInputComponent>(TEXT("InputComp"));

	// Camera
	CameraComp=CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(RootComponent);
	CameraComp->AddRelativeLocation(FVector(30, 0, 70));
	CameraComp->bUsePawnControlRotation=true;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();


}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

