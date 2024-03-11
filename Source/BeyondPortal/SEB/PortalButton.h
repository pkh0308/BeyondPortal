// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PKH/Interface/Interactible.h"
#include "PKH/Props/GrabCube.h"
#include "PortalButton.generated.h"

UCLASS()
class BEYONDPORTAL_API APortalButton : public AActor, public IInteractible
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APortalButton();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;


public:
	UPROPERTY(EditAnywhere)
	class UCapsuleComponent* capsuleComp;
	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent* portalButton;
	UPROPERTY(EditDefaultsOnly)
	UAnimationAsset* pressButtonAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isPressed=false;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSubclassOf<AActor> spawnCube;


	void activeSpawnCube();


	UPROPERTY()
	class AArmMesh* armMesh;

	//Network
	UPROPERTY(EditAnywhere)
	bool isSpawned;


	void SpawnCube();

	UFUNCTION(Server, Reliable)
	void ServerSpawnCube();

	UFUNCTION(NetMulticast, Reliable)
	void MultiSpawnCube();

	

public:
	
	virtual void DoInteraction() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};