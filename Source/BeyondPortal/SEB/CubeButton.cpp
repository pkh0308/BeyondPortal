// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/CubeButton.h"

#include "ArmDoor.h"
#include "CheckOpen.h"
#include "Door.h"
#include "FloorLine.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PKH/Props/GrabCube.h"

// Sets default values
ACubeButton::ACubeButton()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick=true;
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
		cubeButton->SetRelativeLocation(FVector(0, -25, 20));
	}
}

// Called when the game starts or when spawned
void ACubeButton::BeginPlay()
{
	Super::BeginPlay();
	boxComp->OnComponentBeginOverlap.AddDynamic(this, &ACubeButton::OnMyCompBeginOverlap);
	boxComp->OnComponentEndOverlap.AddDynamic(this, &ACubeButton::OnMyCompEndOverlap);
}

// Called every frame
void ACubeButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACubeButton::OnMyCompBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if ( OtherActor->IsA<AGrabCube>() ) {

		//닿으면 cube 색 변경 blue -> orange
		AGrabCube* cube=Cast<AGrabCube>(OtherActor);
		cube->ChangeMaterial(true);

		//눌린 버튼의 tag를 가져와서
		//그 tag와 같은 actor들을 전부 탐색
		TArray<AActor*> FoundActors;

		//충돌한 CubeButton Tag 가져오기
		FString findTag=this->Tags.Num() > 0 ? this->Tags[0].ToString() : TEXT("NoTag"); 
		FName findTagName=FName(*findTag);
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), findTagName, FoundActors);
		FoundActors.Sort([](const AActor& a, const AActor& b)
		{
			return a.GetActorLabel() < b.GetActorLabel();
		});
		float Delay=0.1f;
		for ( auto CurrentActor : FoundActors )
		{
			
			// 찾은 태그 중에 ArmDoor를 찾아서
			if ( CurrentActor->IsA<AArmDoor>() )
			{
				// 딜레이 후에 문 열기
				AArmDoor* Door=Cast<AArmDoor>(CurrentActor);
				FTimerHandle Handle;
				GetWorldTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([Door]()
					{
						Door->isOpened=true;
					}), Delay, false);
				Delay+=0.1f;
			}
			
			else if( CurrentActor->IsA<AFloorLine>() ) //FloorLine 색상 변경
			{
				//색 변경
				FString findColor=CurrentActor->Tags.Num() > 1 ? CurrentActor->Tags[1].ToString() : TEXT("NoTag");
				if( findColor == "blue" )
				{
					CurrentActor->SetActorHiddenInGame(true);
				}
				else if ( findColor == "orange" )
				{
					CurrentActor->SetActorHiddenInGame(false);
				}

				
			}
			else if(CurrentActor->IsA< ACheckOpen>() ) //checkopen 보드 색상 변경
			{
				ACheckOpen* co=Cast<ACheckOpen>(CurrentActor);
				co->checkOpen->SetMaterial(0, co->mat);
			}
			else if(CurrentActor->IsA< ADoor>() )
			{
				ADoor* door=Cast<ADoor>(CurrentActor);
				door->RPC_Server_OpenDoor();
			}
		}

	}


}

void ACubeButton::OnMyCompEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if ( OtherActor->IsA<AGrabCube>() ) {

		//닿으면 cube 색 변경 orange -> blue
		AGrabCube* cube=Cast<AGrabCube>(OtherActor);
		cube->ChangeMaterial(false);

		TArray<AActor*> FoundActors;
		FString findTag=this->Tags.Num() > 0 ? this->Tags[0].ToString() : TEXT("NoTag");
		FName findTagName=FName(*findTag);
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), findTagName, FoundActors);

		float Delay=0.1f;
		for ( auto CurrentActor : FoundActors )
		{

			// 찾은 태그 중에 ArmDoor를 찾아서
			if ( CurrentActor->IsA<AArmDoor>() )
			{
				//문 닫기

				// 딜레이 후에 문 열기
				AArmDoor* Door=Cast<AArmDoor>(CurrentActor);
				FTimerHandle Handle;
				GetWorldTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([Door]()
					{
						Door->isClosed=true;
					}), Delay, false);
				Delay+=0.1f;

			}
			else if ( CurrentActor->IsA<AFloorLine>() ) //FloorLine 색상 변경
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

void ACubeButton::OpenNextDoor(int32 i)
{
	if(armDoorArray.Num() > 0 )
	{
		UE_LOG(LogTemp, Error, TEXT("왜 안열리냐.. 이거 인덱스는 %d"), i);
		// 지정된 인덱스의 문 열기
		Cast<AArmDoor>(armDoorArray[i])->isOpened=true;
	}
		
	
}

/*
bool ACubeButton::SortByName(const AActor& currentActorA, const AActor& currentActorB)
{
	return currentActorA.GetActorLabel() < currentActorB.GetActorLabel();
}
*/


