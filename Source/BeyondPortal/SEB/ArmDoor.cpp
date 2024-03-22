// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/ArmDoor.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AArmDoor::AArmDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// mesh 생성
	armDoor=CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("armDoor"));
	this->SetRootComponent(armDoor);


	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/SEB/Resources/Mesh/Arm/arm_interior_192_model.arm_interior_192_model'"));
	if ( tempMesh.Succeeded() )
	{
		armDoor->SetSkeletalMesh(tempMesh.Object);
		armDoor->SetAnimationMode(EAnimationMode::AnimationSingleNode);
		
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> SFX_OpenDoorRef(TEXT("/Script/Engine.SoundWave'/Game/SEB/Resources/Sounds/DoorOpen.DoorOpen'"));
	if ( SFX_OpenDoorRef.Object )
	{
		SFX_OpenDoor=SFX_OpenDoorRef.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<USoundBase> SFX_CloseDoorRef(TEXT("/Script/Engine.SoundWave'/Game/SEB/Resources/Sounds/Portal-2-Sound-Effects-Door-Close.Portal-2-Sound-Effects-Door-Close'"));
	if ( SFX_CloseDoorRef.Object )
	{
		SFX_CloseDoor=SFX_CloseDoorRef.Object;
	}
}

// Called when the game starts or when spawned
void AArmDoor::BeginPlay()
{
	Super::BeginPlay();
	armDoor->PlayAnimation(armDoorIdleAnim, true);
	
}

// Called every frame
void AArmDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RPC_Server_MoveDoor();
}

void AArmDoor::RPC_Server_MoveDoor_Implementation()
{
	RPC_Multi_MoveDoor();
}

void AArmDoor::RPC_Multi_MoveDoor_Implementation()
{
	if ( isOpened )
	{
		isClosed=false;
		armDoor->PlayAnimation(armDoorOpenAnim, false);
		UGameplayStatics::PlaySound2D(GetWorld(),SFX_OpenDoor, 0.3f );
		isOpened=false;
	}
	if ( isClosed )
	{

		armDoor->PlayAnimation(armDoorCloseAnim, false);
		UGameplayStatics::PlaySound2D(GetWorld(),SFX_CloseDoor, 0.3f );

		isClosed=false;
		isOpened=false;
	}
}

void AArmDoor::closeDoor(AActor* findDoor, float Delay)
{

	// 딜레이 후에 문 열기
	AArmDoor* Door=Cast<AArmDoor>(findDoor);
	UAnimationAsset* CloseAnim=armDoorCloseAnim;
	TObjectPtr<class USkeletalMeshComponent> arm=armDoor;
	FTimerHandle Handle;
	UGameplayStatics::PlaySound2D(GetWorld(),SFX_CloseDoor, 0.3f );

	GetWorldTimerManager().SetTimer(Handle, [Door, arm, CloseAnim]()
		{
			arm->PlayAnimation(CloseAnim, false);

		}, Delay, false);


	armDoor->PlayAnimation(armDoorCloseAnim, false);
	isClosed=false;
	isOpened=false;
}

void AArmDoor::openDoor(AActor* findDoor, float Delay)
{
	// 딜레이 후에 문 열기
	AArmDoor* Door=Cast<AArmDoor>(findDoor);
	UAnimationAsset* OpenAnim=armDoorOpenAnim;
	TObjectPtr<class USkeletalMeshComponent> arm=armDoor;
	FTimerHandle Handle;
	TObjectPtr<class USoundBase> OpenDoorSound = SFX_OpenDoor;
	UGameplayStatics::PlaySound2D(Door,OpenDoorSound, 0.3f );
	GetWorldTimerManager().SetTimer(Handle, [Door, arm, OpenAnim, OpenDoorSound]()
		{
			arm->PlayAnimation(OpenAnim, false);
		

		}, Delay, false);
	

	isClosed=false;
	//armDoor->PlayAnimation(armDoorOpenAnim, false);
	isOpened=false;
}

void AArmDoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AArmDoor, armDoorIdleAnim);
	DOREPLIFETIME(AArmDoor, armDoorOpenAnim);
	DOREPLIFETIME(AArmDoor, armDoorCloseAnim);

}

