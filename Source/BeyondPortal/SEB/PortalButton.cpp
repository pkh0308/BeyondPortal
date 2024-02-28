// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/PortalButton.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "PKH/Player/PlayerCharacter.h"

// Sets default values
APortalButton::APortalButton()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// capsuleComp를 생성
	capsuleComp=CreateDefaultSubobject<UCapsuleComponent>(TEXT("capsuleComp"));
	// 루트 컴포넌트로 변경
	this->SetRootComponent(capsuleComp);
	capsuleComp->SetCapsuleHalfHeight(100);
	capsuleComp->SetCapsuleRadius(30);

	// mesh 생성
	portalButton=CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("portalButton"));
	// 루트에 붙임
	portalButton->SetupAttachment(RootComponent);

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/SEB/Resources/Mesh/PedestalButton/PortalButton.PortalButton'"));
	if ( tempMesh.Succeeded() )
	{
		portalButton->SetSkeletalMesh(tempMesh.Object);
		portalButton->SetRelativeLocation(FVector(0, 0, -70));
		portalButton->SetRelativeScale3D(FVector(0.03f));
	}
}

// Called when the game starts or when spawned
void APortalButton::BeginPlay()
{
	Super::BeginPlay();
	capsuleComp->OnComponentBeginOverlap.AddDynamic(this, &APortalButton::OnMyCompBeginOverlap);
}

// Called every frame
void APortalButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APortalButton::OnMyCompBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Player가 앞에 와서
	if (OtherActor->IsA<APlayerCharacter>()) {
		

		//E 버튼을 누르면

		//버튼 종류 식별 (SpawnCube / OpenDoor)

		// 버튼 애니메이션 play
		portalButton->PlayAnimation(pressButtonAnim, false);
		//isPressed를 true로 바꾸고
		isPressed=true;
		//isPressed가 true라면
		if(isPressed )
		{
			UE_LOG(LogTemp, Warning, TEXT("portalbutton"));
		}
		
	}
}



