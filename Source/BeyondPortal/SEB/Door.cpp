// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/Door.h"

#include "PortalButton.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PKH/Player/PlayerCharacter.h"

// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/*// capsuleComp를 생성
	boxComp=CreateDefaultSubobject<UBoxComponent>(TEXT("boxComp"));
	// 루트 컴포넌트로 변경
	this->SetRootComponent(boxComp);*/

	// mesh 생성
	door=CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("door"));
	// 루트로 설정
	/*door->SetupAttachment(RootComponent);*/
	this->SetRootComponent(RootComponent);

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/SEB/Resources/Mesh/PortalDoor/PortalDoor.PortalDoor'"));
	if ( tempMesh.Succeeded() )
	{
		door->SetSkeletalMesh(tempMesh.Object);
		door->SetRelativeScale3D(FVector(0.004f));
	}
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();

	/*boxComp->OnComponentBeginOverlap.AddDynamic(this, &ADoor::OnMyCompBeginOverlap);*/

	//portalButton 가져와서 저장
	/*AActor* foundActor = UGameplayStatics::GetActorOfClass(GetWorld(), APortalButton::StaticClass());
	portalButton=Cast<APortalButton>(foundActor);*/
	
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//portalButton이 눌렸다면
	/*if(portalButton->isPressed )
	{
		UE_LOG(LogTemp, Error, TEXT("door"));
		door->PlayAnimation(openDoorAnim, false);
		portalButton->isPressed=false;
	}*/
}

void ADoor::OnMyCompBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Player가 앞에 와서 충돌하면
	/*if ( OtherActor->IsA<APlayerCharacter>() ) {

		door->PlayAnimation(openDoorAnim, false);
		
		
	}*/
}

