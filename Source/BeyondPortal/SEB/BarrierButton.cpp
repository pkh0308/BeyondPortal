// Fill out your copyright notice in the Description page of Project Settings.


#include "BarrierButton.h"

#include "FloorLine.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PKH/Player/PlayerCharacter.h"
#include "SEB/Barrier.h"

// Sets default values
ABarrierButton::ABarrierButton()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	boxComp=CreateDefaultSubobject<UBoxComponent>(TEXT("boxComp"));
	this->SetRootComponent(boxComp);

	boxComp->SetBoxExtent(FVector(32, 32, 15));

	barrierButton=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("barrierButton"));
	barrierButton->SetupAttachment(RootComponent);

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Game/SEB/Resources/Mesh/Button/portal_button.portal_button'"));
	if ( tempMesh.Succeeded() )
	{
		barrierButton->SetStaticMesh(tempMesh.Object);
		barrierButton->SetRelativeRotation(FRotator(0, 0, 90));
		
	}
}

// Called when the game starts or when spawned
void ABarrierButton::BeginPlay()
{
	Super::BeginPlay();
	boxComp->OnComponentBeginOverlap.AddDynamic(this, &ABarrierButton::OnMyCompBeginOverlap);
	boxComp->OnComponentEndOverlap.AddDynamic(this, &ABarrierButton::OnMyCompEndOverlap);
}

void ABarrierButton::OnMyCompBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if ( OtherActor->IsA<APlayerCharacter>() ) {

		TArray<AActor*> FoundActors;

		//Barrier없어짐.
		FString findTag=this->Tags.Num() > 0 ? this->Tags[0].ToString() : TEXT("NoTag");//DestroyBarrier
		FName findTagName=FName(*findTag);
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), findTagName, FoundActors);
		for ( auto CurrentActor : FoundActors )
		{
			// 찾은 태그 중에 ArmDoor를 찾아서
			if ( CurrentActor->IsA<ABarrier>() )
			{
				CurrentActor->SetActorHiddenInGame(true);
				ABarrier* b=Cast<ABarrier>(CurrentActor);
				b->boxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}

			else if ( CurrentActor->IsA<AFloorLine>() )
			{

				FString findColor=CurrentActor->Tags.Num() > 1 ? CurrentActor->Tags[1].ToString() : TEXT("NoTag");
				if ( findColor == "blue" )
				{
					CurrentActor->SetActorHiddenInGame(true);
				}
				else if ( findColor == "orange" )
				{
					CurrentActor->SetActorHiddenInGame(false);
				}
			}
		}

	}
}

void ABarrierButton::OnMyCompEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if ( OtherActor->IsA<APlayerCharacter>() ) {

		TArray<AActor*> FoundActors;

		//Barrier없어짐.
		FString findTag=this->Tags.Num() > 0 ? this->Tags[0].ToString() : TEXT("NoTag");//DestroyBarrier
		FName findTagName=FName(*findTag);
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), findTagName, FoundActors);
		for ( auto CurrentActor : FoundActors )
		{
			// 찾은 태그 중에 ArmDoor를 찾아서
			if ( CurrentActor->IsA<ABarrier>() )
			{
				CurrentActor->SetActorHiddenInGame(false);
				ABarrier* b=Cast<ABarrier>(CurrentActor);
				b->boxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			}

			else if ( CurrentActor->IsA<AFloorLine>() )
			{

				FString findColor=CurrentActor->Tags.Num() > 1 ? CurrentActor->Tags[1].ToString() : TEXT("NoTag");
				if ( findColor == "blue" )
				{
					CurrentActor->SetActorHiddenInGame(false);
				}
				else if ( findColor == "orange" )
				{
					CurrentActor->SetActorHiddenInGame(true);
				}
			}
		}


	}
}

// Called every frame
void ABarrierButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

