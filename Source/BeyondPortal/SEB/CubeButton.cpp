// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/CubeButton.h"

#include "Components/BoxComponent.h"

// Sets default values
ACubeButton::ACubeButton()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// capsuleComp를 생성
	boxComp=CreateDefaultSubobject<UBoxComponent>(TEXT("boxComp"));
	// 루트 컴포넌트로 변경
	this->SetRootComponent(boxComp);
	

	// mesh 생성
	cubeButton=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("cubeButton"));
	// 루트에 붙임
	cubeButton->SetupAttachment(RootComponent);

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Game/SEB/Resources/Mesh/CubeButton/CubeButton.CubeButton'"));
	if ( tempMesh.Succeeded() )
	{
		cubeButton->SetStaticMesh(tempMesh.Object);
		cubeButton->SetRelativeScale3D(FVector(0.03f));
	}
}

// Called when the game starts or when spawned
void ACubeButton::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACubeButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

