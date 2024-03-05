// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/Barrier.h"

#include "PortalButton.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PKH/Props/GrabCube.h"

// Sets default values
ABarrier::ABarrier()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	boxComp=CreateDefaultSubobject<UBoxComponent>(TEXT("boxComp"));
	this->SetRootComponent(boxComp);
	barrier=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("barrier"));
	barrier->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABarrier::BeginPlay()
{
	Super::BeginPlay();
	boxComp->OnComponentBeginOverlap.AddDynamic(this, &ABarrier::OnMyCompBeginOverlap);
	
	
}

// Called every frame
void ABarrier::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABarrier::OnMyCompBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ( OtherActor->IsA<AGrabCube>() ) {
		
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("닿았다"));
		// 결계랑 큐브랑 닿으면 큐브 제거
		OtherActor->Destroy();
		// 이펙트 추가
		// 다시 스폰될 수 있도록 하기.
		
		
	}
}

