// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PKH/Interface/Interactible.h"
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

	//충돌처리
	UFUNCTION()
	void OnMyCompBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	UPROPERTY(EditAnywhere)
	class UCapsuleComponent* capsuleComp;
	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent* portalButton;
	UPROPERTY(EditDefaultsOnly)
	UAnimationAsset* pressButtonAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isPressed = false;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSubclassOf<AActor> spawnCube;

	void activeSpawnCube();

	UPROPERTY(EditAnywhere)
	bool isSpawned;

	UPROPERTY()
	class AArmMesh* armMesh;



public:
	virtual void DoInteraction() override;
};
