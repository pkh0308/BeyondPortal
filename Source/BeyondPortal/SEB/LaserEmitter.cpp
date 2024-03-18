// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/LaserEmitter.h"

#include "Components/ArrowComponent.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ALaserEmitter::ALaserEmitter()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick=true;

	LaserEmitter=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaserEmitter"));
	SetRootComponent(LaserEmitter);

	Cube=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube"));
	Cube->SetupAttachment(RootComponent);

	Arrow=CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(RootComponent);


	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Game/SEB/Blueprints/Laser/LaserEmitter/LaserEmitter.LaserEmitter'"));
	if ( tempMesh.Succeeded() )
	{
		LaserEmitter->SetStaticMesh(tempMesh.Object);
		LaserEmitter->SetRelativeRotation(FRotator(90, 0, 0));
		LaserEmitter->SetRelativeScale3D(FVector(0.01f));
	}
}

// Called when the game starts or when spawned
void ALaserEmitter::BeginPlay()
{
	Super::BeginPlay();
	

}

// Called every frame
void ALaserEmitter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, [&]()
	{
		CastLight(Arrow->GetComponentLocation(), Arrow->GetForwardVector(), CastDistance);
	},0.01, true);
}

void ALaserEmitter::CastLight(FVector CastOrigin, FVector CastDirection, float CastDistances)
{
	FVector L_CastOrigin;
	FVector L_CastDirection;
	float L_CastDistance;
	bool L_Continue=true;



	L_CastOrigin=CastOrigin;
	L_CastDirection=CastDirection;
	L_CastDistance=CastDistances;

	FVector start=L_CastOrigin;
	FVector end=L_CastOrigin + L_CastDirection * L_CastDistance;

	FHitResult HitResult;
	bool isReturn;
	

	while(L_Continue )
	{
		isReturn = UKismetSystemLibrary::LineTraceSingle(GetWorld(), start, end, UEngineTypes::ConvertToTraceType(ECC_Visibility), true, TArray<AActor*>(), EDrawDebugTrace::ForDuration, HitResult, true, FColor::Red, FColor::Green, 0.1);

		/*if(isReturn )
		{
			int SectionIndex=0;
			L_CastOrigin=HitResult.ImpactPoint;
			L_CastDirection=UKismetMathLibrary::MirrorVectorByNormal(L_CastDirection, HitResult.ImpactNormal);
			L_Continue=true;
			
		}
		else
		{
			L_Continue=false;
		}*/
		
		
	}

	



}

