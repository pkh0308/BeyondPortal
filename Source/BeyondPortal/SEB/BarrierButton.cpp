// Fill out your copyright notice in the Description page of Project Settings.


#include "BarrierButton.h"

#include "CheckOpen.h"
#include "FloorLine.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
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

	static ConstructorHelpers::FObjectFinder<USoundBase> SFX_BarrierButtonDownRef(TEXT("/Script/Engine.SoundWave'/Game/SEB/Resources/Sounds/barrier2.barrier2'"));
	if ( SFX_BarrierButtonDownRef.Object )
	{
		SFX_BarrierButtonDown=SFX_BarrierButtonDownRef.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> SFX_BarrierButtonRef(TEXT("/Script/Engine.SoundWave'/Game/SEB/Resources/Sounds/select_2-96163.select_2-96163'"));
	if ( SFX_BarrierButtonRef.Object )
	{
		SFX_BarrierButton=SFX_BarrierButtonRef.Object;
	}
	bReplicates=true;
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
		isCheck=true;
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
				UGameplayStatics::PlaySound2D(GetWorld(),SFX_BarrierButtonDown, 0.3f );

			}

			else if ( CurrentActor->IsA<AFloorLine>() )
			{
				UGameplayStatics::PlaySound2D(GetWorld(),SFX_BarrierButton, 0.3f );

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

			else if ( CurrentActor->IsA< ACheckOpen>() ) //checkopen 보드 색상 변경
			{
				ACheckOpen* co=Cast<ACheckOpen>(CurrentActor);
				co->checkOpen->SetMaterial(0, co->mat);
				isCheck=true;
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
				UGameplayStatics::PlaySound2D(GetWorld(),SFX_BarrierButtonDown, 0.3f );

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

			else if ( CurrentActor->IsA< ACheckOpen>() ) //checkopen 보드 색상 변경
			{
				ACheckOpen* co=Cast<ACheckOpen>(CurrentActor);
				co->checkOpen->SetMaterial(0, co->mat2);
			}
		}
	}
}

// Called every frame
void ABarrierButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void ABarrierButton::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABarrierButton, cnt);
}
