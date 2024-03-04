// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArmDoor.generated.h"

UCLASS()
class BEYONDPORTAL_API AArmDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AArmDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class USkeletalMeshComponent> armDoor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool isOpened=false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool isClosed=false;
	
	UPROPERTY(EditDefaultsOnly)
	UAnimationAsset* armDoorIdleAnim;

	UPROPERTY(EditDefaultsOnly)
	UAnimationAsset* armDoorOpenAnim;

	UPROPERTY(EditDefaultsOnly)
	UAnimationAsset* armDoorCloseAnim;

};
